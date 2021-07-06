/*
 * $Id: ezligolw.c,v 1.4 2008/07/31 08:28:42 kipp Exp $
 *
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
#include <libezligolw/ezligolw.h>


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
	int n;

	array = malloc(sizeof(*array));
	if(!array)
		return NULL;

	array->name = ligolw_strip_array_name(ezxml_attr(elem, "Name"));
	array->type = ligolw_type_name_to_enum(ezxml_attr(elem, "Type"));
	array->delimiter = '\0';
	array->n_dims = 0;
	array->dims = NULL;
	array->data = NULL;

	for(n = 1, dim = ezxml_child(elem, "Dim"); dim; dim = dim->next) {
		array->dims = realloc(array->dims, (array->n_dims + 1) * sizeof(*array->dims));

		array->dims[array->n_dims].n = strtoll(dim->txt, NULL, 0);
		n *= array->dims[array->n_dims].n;
		/* each is set to NULL if the attribute does not exist */
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

	array->data = malloc(n * ligolw_type_enum_to_size(array->type));
	if(!array->data) {
		free(array->dims);
		free(array);
		return NULL;
	}

	array->delimiter = *ezxml_attr(stream, "Delimiter");

	for(n = 0, txt = stream->txt; txt && *txt; n++) {
		char *end, *next;

		ligolw_next_token(&txt, &end, &next, array->delimiter);

		switch(array->type) {
		case ligolw_cell_type_int_2s:
			((int16_t *) array->data)[n] = strtoll(txt, NULL, 0);
			break;

		case ligolw_cell_type_int_2u:
			((uint16_t *) array->data)[n] = strtoull(txt, NULL, 0);
			break;

		case ligolw_cell_type_int_4s:
			((int32_t *) array->data)[n] = strtoll(txt, NULL, 0);
			break;

		case ligolw_cell_type_int_4u:
			((uint32_t *) array->data)[n] = strtoull(txt, NULL, 0);
			break;

		case ligolw_cell_type_int_8s:
			((int64_t *) array->data)[n] = strtoll(txt, NULL, 0);
			break;

		case ligolw_cell_type_int_8u:
			((uint64_t *) array->data)[n] = strtoull(txt, NULL, 0);
			break;

		case ligolw_cell_type_real_4:
			((float *) array->data)[n] = strtof(txt, NULL);
			break;

		case ligolw_cell_type_real_8:
			((double *) array->data)[n] = strtod(txt, NULL);
			break;

		case ligolw_cell_type_complex_8: {
			float re, im;
			re = strtof(txt, &txt);
			/* skip "+i" */
			txt += 2;
			im = strtof(txt, NULL);
			((float complex *) array->data)[n] = re + im * I;
			break;
		}

		case ligolw_cell_type_complex_16: {
			double re, im;
			re = strtod(txt, &txt);
			/* skip "+i" */
			txt += 2;
			im = strtod(txt, NULL);
			((double complex *) array->data)[n] = re + im * I;
			break;
		}

		default:
			/* non-numeric types not supported */
			free(array->data);
			free(array->dims);
			free(array);
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
 * Find an ezxml_t Array element in a document.
 */


ezxml_t ligolw_array_get(ezxml_t xmldoc, const char *name)
{
	int n = strlen(name);
	ezxml_t elem;

	for(elem = ezxml_child(xmldoc, "Table"); elem; elem = elem->next)
		if(!strncmp(ligolw_strip_array_name(ezxml_attr(elem, "Name")), name, n))
			break;

	return elem;
}
