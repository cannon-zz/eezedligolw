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


#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
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
		/* FIXME:  check for failure (extremely unlikely) */
		array->dims = realloc(array->dims, (array->n_dims + 1) * sizeof(*array->dims));

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

	array->delimiter = *ezxml_attr(stream, "Delimiter");

	for(data = array->data, txt = stream->txt; txt && *txt; data += stride) {
		union ligolw_cell cell;
		char *end, *next;

		ligolw_next_token(&txt, &end, &next, array->delimiter);

		/* we have confirmed above that array->type is a numeric
		 * type, so we don't need to bother null-terminating the
		 * token before calling _cell_from_txt because parsing will
		 * automatically stop at the end of the nuber, and also we
		 * know the _to_c() function will not do something weird to
		 * the contents of the array. */
		if(!ligolw_cell_from_txt(&cell, array->type, txt) || ligolw_cell_to_c(&cell, array->type, data)) {
			ligolw_array_free(array);
			return NULL;
		}

		/* advance to next token */
		txt = next;
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


ezxml_t ligolw_array_get(ezxml_t xmldoc, const char *name)
{
	int n = name ? strlen(name) : 0;
	ezxml_t elem;

	for(elem = ezxml_child(xmldoc, "Array"); elem; elem = elem->next)
		if(!n || !strncmp(ligolw_strip_array_name(ezxml_attr(elem, "Name")), name, n))
			break;

	return elem;
}
