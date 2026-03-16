/*
 * Copyright (C) 2007  Kipp Cannon
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <assert.h>
#include <byteswap.h>
#include <complex.h>
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <b64/cdecode.h>
#include <./stream.h>
#include <libezligolw/ezligolw.h>


/*
 * check that a type is a numeric type
 */


static int ligolw_cell_type_is_numeric(enum ligolw_cell_type type)
{
	static const enum ligolw_cell_type numeric_types[] = {
		ligolw_cell_type_int_2s,
		ligolw_cell_type_int_2u,
		ligolw_cell_type_int_4s,
		ligolw_cell_type_int_4s,
		ligolw_cell_type_int_4u,
		ligolw_cell_type_int_8s,
		ligolw_cell_type_int_8u,
		ligolw_cell_type_real_4,
		ligolw_cell_type_real_8,
		ligolw_cell_type_complex_8,
		ligolw_cell_type_complex_16
	};
	int i;

	for(i = 0; i < sizeof(numeric_types) / sizeof(*numeric_types) ; i++)
		if(type == numeric_types[i])
			/* found it */
			return 1;
	/* didn't find it */
	return 0;
}


/*
 * Extract the meaningful portion of an Array name.  Returns a pointer to
 * the last colon-delimited substring before an optional ":array" suffix.
 */


static const char *ligolw_strip_array_name(const char *Name)
{
	return ligolw_strip_name(Name, "array");
}


/*
 * Extract the meaningful portion of a Dim name.  Returns a pointer to
 * the last colon-delimited substring before an optional ":dim" suffix.
 */


static const char *ligolw_strip_dim_name(const char *Name)
{
	return ligolw_strip_name(Name, "dim");
}


/*
 * Parse an ezxml_t Array element into a struct ligolw_array structure.
 *
 * ligolw_array_parse() returns the pointer to the new struct ligolw_array
 * structure on success, NULL on failure.
 */


struct ligolw_array *ligolw_array_parse(ezxml_t elem)
{
	struct ligolw_array *array;
	char *txt;
	ezxml_t dim;
	ezxml_t stream;
	char *data;
	int stride;
	int n;
	enum ligolw_stream_encoding encoding;

	/* this simplifies error checking in calling code */
	if(!elem)
		return NULL;

	array = malloc(sizeof(*array));
	if(!array)
		return NULL;

	/* FIXME:  should should :array suffix? */
	array->name = ligolw_strip_array_name(ezxml_attr(elem, "Name"));
	array->type = ligolw_type_name_to_enum(ezxml_attr(elem, "Type"));
	array->delimiter = '\0';
	array->n_dims = 0;
	array->dims = NULL;
	array->data = NULL;

	/* from here on, ligolw_array_free() can be used for memory cleanup */

	if(!ligolw_cell_type_is_numeric(array->type)) {
		/* non-numeric types not supported */
		ligolw_array_free(array);
		return NULL;
	}

	for(n = 1, dim = ezxml_child(elem, "Dim"); dim; dim = dim->next) {
		array->dims = realloc(array->dims, (array->n_dims + 1) * sizeof(*array->dims));
		/* not great error handling, but failure is *extremely* unlikely */
		assert(array->dims != NULL);

		array->dims[array->n_dims].n = strtoll(dim->txt, NULL, 0);
		n *= array->dims[array->n_dims].n;
		array->dims[array->n_dims].name = ligolw_strip_dim_name(ezxml_attr(dim, "Name"));
		array->dims[array->n_dims].unit = ezxml_attr(dim, "Unit");
		array->dims[array->n_dims].start = ezxml_attr(dim, "Start");
		array->dims[array->n_dims].scale = ezxml_attr(dim, "Scale");

		array->n_dims++;
	}

	stream = ezxml_child(elem, "Stream");
	if(!stream) {
		/* DTD allows Array to have 0 Stream children */
		return array;
	}

	stride = ligolw_type_enum_to_size(array->type);
	array->data = malloc(n * stride);
	if(!array->data) {
		ligolw_array_free(array);
		return NULL;
	}

	array->delimiter = ligolw_stream_delimiter(stream);
	if(array->delimiter < 0) {
		/* invalid delimiter */
		ligolw_array_free(array);
		return NULL;
	}

	switch(encoding = ligolw_stream_check_encoding(stream)) {
	case ligolw_stream_enc_text:
		for(data = array->data, txt = stream->txt; txt && *txt; data += stride) {
			union ligolw_cell cell;
			char *start, *end;

			ligolw_stream_next_token(&txt, &start, &end, array->delimiter);

			/* we have confirmed above that array->type is a
			 * numeric type, so we don't need to bother
			 * null-terminating the token before calling
			 * _cell_from_txt because parsing will
			 * automatically stop at the end of the number.  we
			 * also know we will not be given ownership of a
			 * string or blob pointer that requires free(). */
			if(!ligolw_cell_from_txt(&cell, array->type, start) || ligolw_cell_to_c(&cell, array->type, data)) {
				ligolw_array_free(array);
				return NULL;
			}
		}
		break;

	case ligolw_stream_enc_b64be:
	case ligolw_stream_enc_b64le: {
		base64_decodestate b64state;
		base64_init_decodestate(&b64state);
		/* decode the base64 data */
		if(base64_decode_block(stream->txt, strlen(stream->txt), array->data, &b64state) != n * stride) {
			/* decoded size did not match array size.  we might
			 * have corrupted memory, but other than try to
			 * clean up and return an error code there's not
			 * much we can do */
			ligolw_array_free(array);
			return NULL;
		}
		/* correct the endianness */
		if(__BYTE_ORDER != ((encoding & ligolw_stream_enc_LittleEndian) ? __LITTLE_ENDIAN : __BIG_ENDIAN)) {
			int i;
			switch(array->type) {
			case ligolw_cell_type_int_2s:
			case ligolw_cell_type_int_2u:
				for(i = 0; i < n; i++)
					((uint16_t *) array->data)[i] = bswap_16(((uint16_t *) array->data)[i]);
				break;

			case ligolw_cell_type_int_4s:
			case ligolw_cell_type_int_4u:
			case ligolw_cell_type_real_4:
				for(i = 0; i < n; i++)
					((uint32_t *) array->data)[i] = bswap_32(((uint32_t *) array->data)[i]);
				break;

			case ligolw_cell_type_int_8s:
			case ligolw_cell_type_int_8u:
			case ligolw_cell_type_real_8:
				for(i = 0; i < n; i++)
					((uint64_t *) array->data)[i] = bswap_64(((uint64_t *) array->data)[i]);
				break;

			case ligolw_cell_type_complex_8:
				/* single precision complex numbers are pairs of 4
				 * byte numbers, not single 8 byte numbers */
				for(i = 0; i < 2 * n; i++)
					((uint32_t *) array->data)[i] = bswap_32(((uint32_t *) array->data)[i]);
				break;

			case ligolw_cell_type_complex_16:
				/* double precision complex numbers are pairs of 8
				 * byte numbers, not single 16 byte numbers */
				for(i = 0; i < 2 * n; i++)
					((uint64_t *) array->data)[i] = bswap_64(((uint64_t *) array->data)[i]);
				break;

			default:
				/* should not get here:  we confirmed the Stream
				 * contains one of the above numeric types */
				ligolw_array_free(array);
				return NULL;
			}
		}
		break;
	}

	default:
		ligolw_array_free(array);
		return NULL;
	}

	return array;
}


/*
 * Free a struct ligolw_array.
 */


void ligolw_array_free(struct ligolw_array *array)
{

	if(array) {
		free(array->dims);
		free(array->data);
	}
	free(array);
}


/*
 * Find an ezxml_t Array element in a document.  If name is NULL the first
 * element found is reported, otherwise the elment's Name attribute must
 * match name (ignoring an optional :... suffix).
 */


ezxml_t ligolw_array_get(ezxml_t elem, const char *name)
{
	return ligolw_elem_iter(elem, "Array", name);
}
