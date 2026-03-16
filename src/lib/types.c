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


#include <stdio.h>
#include <string.h>
#include <b64/cdecode.h>
#include <b64/cencode.h>
#include <ezligolw/ezligolw.h>


/*
 * Convert a LIGO Light Weight type name string to/from a numeric type
 * index.
 */


static const struct name_to_enum {
	const char *name;
	enum ligolw_cell_type type;
} name_to_enum[] = {
	{"char_s", ligolw_cell_type_char_s},
	{"char_v", ligolw_cell_type_char_v},
	{"ilwd:char", ligolw_cell_type_ilwdchar},
	{"ilwd:char_u", ligolw_cell_type_ilwdchar_u},
	{"lstring", ligolw_cell_type_lstring},
	{"string", ligolw_cell_type_lstring},
	{"int_2s", ligolw_cell_type_int_2s},
	{"int_2u", ligolw_cell_type_int_2u},
	{"int_4s", ligolw_cell_type_int_4s},
	{"int", ligolw_cell_type_int_4s},
	{"int_4u", ligolw_cell_type_int_4u},
	{"int_8s", ligolw_cell_type_int_8s},
	{"int_8u", ligolw_cell_type_int_8u},
	{"real_4", ligolw_cell_type_real_4},
	{"float", ligolw_cell_type_real_4},
	{"real_8", ligolw_cell_type_real_8},
	{"double", ligolw_cell_type_real_8},
	{"complex_8", ligolw_cell_type_complex_8},
	{"complex_16", ligolw_cell_type_complex_16},
	{NULL, -1}
};


enum ligolw_cell_type ligolw_type_name_to_enum(const char *name)
{
	const struct name_to_enum *n_to_e;

	if(!name)
		return -1;

	for(n_to_e = name_to_enum; n_to_e->name; n_to_e++)
		if(!strcmp(n_to_e->name, name))
			/* found it */
			return n_to_e->type;

	/* unrecognized type */
	return -1;
}


const char *ligolw_type_enum_to_name(enum ligolw_cell_type t)
{
	const struct name_to_enum *n_to_e;

	for(n_to_e = name_to_enum; n_to_e->name; n_to_e++)
		if(n_to_e->type == t)
			/* found it */
			return n_to_e->name;

	/* unrecognized type */
	return NULL;
}


size_t ligolw_type_enum_to_size(enum ligolw_cell_type t)
{
	switch(t) {
	case ligolw_cell_type_char_s:
	case ligolw_cell_type_char_v:
	case ligolw_cell_type_ilwdchar:
	case ligolw_cell_type_ilwdchar_u:
	case ligolw_cell_type_lstring:
	default:
		/* size not defined */
		return -1;

	case ligolw_cell_type_int_2s:
	case ligolw_cell_type_int_2u:
		return 2;

	case ligolw_cell_type_int_4s:
	case ligolw_cell_type_int_4u:
	case ligolw_cell_type_real_4:
		return 4;

	case ligolw_cell_type_int_8s:
	case ligolw_cell_type_int_8u:
	case ligolw_cell_type_real_8:
	case ligolw_cell_type_complex_8:
		return 8;

	case ligolw_cell_type_complex_16:
		return 16;
	}
}


/*
 * populates a union ligolw_cell object by parsing the text contents of an
 * ezxml_t element as the given type.  returns the address of the union
 * ligolw_cell object on success, NULL on error.
 *
 * the data for string and blob types is decoded and returned copied into a
 * newly allocated buffer.  the calling code owns the memory and must
 * free() it when finished with it.
 */


union ligolw_cell *ligolw_cell_from_txt(union ligolw_cell *cell, enum ligolw_cell_type type, const char *txt)
{
	/* FIXME:  implement error checking */
	switch(type) {
	case ligolw_cell_type_char_s:
	case ligolw_cell_type_char_v:
	case ligolw_cell_type_ilwdchar:
	case ligolw_cell_type_ilwdchar_u:
	case ligolw_cell_type_lstring:
		cell->as_string = strdup(txt);
		break;

	case ligolw_cell_type_blob: {
		size_t n = strlen(txt);
		base64_decodestate b64state;
		base64_init_decodestate(&b64state);
		cell->as_blob.len = n; /*FIXME: base64_decode_maxlength(n)*/
		cell->as_blob.data = malloc(cell->as_blob.len);
		base64_decode_block(txt, n, cell->as_blob.data, &b64state);
		break;
	}

	case ligolw_cell_type_int_2s:
	case ligolw_cell_type_int_4s:
	case ligolw_cell_type_int_8s:
		cell->as_int = strtoll(txt, NULL, 0);
		break;

	case ligolw_cell_type_int_2u:
	case ligolw_cell_type_int_4u:
	case ligolw_cell_type_int_8u:
		cell->as_uint = strtoull(txt, NULL, 0);
		break;

	case ligolw_cell_type_real_4:
	case ligolw_cell_type_real_8:
		cell->as_double = strtod(txt, NULL);
		break;

	case ligolw_cell_type_complex_8:
	case ligolw_cell_type_complex_16: {
		double re, im;
		char *end;
		re = strtod(txt, &end);
		/* skip "+i" */
		end += 2;
		im = strtod(end, NULL);
		cell->as_double_complex = re + im * I;
		break;
	}
	default:
		/* unrecognized type enum */
		return NULL;
	}

	return cell;
}


/*
 * Print the contents of the a union ligolw_cell object to a newly
 * allocated string encoded following the LIGO Light-Weight XML convention.
 * The result is suitable for printing into a Table element, or Array
 * element.  Probably reasonable to use it for a Param element, also, but
 * be aware of the confusion surrounding quoting of strings in Param
 * elements.
 *
 * The return value is the address of a newly allocated buffer (which the
 * calling code must free when finished with it), or NULL if an error
 * occurs.
 */


char *ligolw_cell_to_txt(union ligolw_cell cell, enum ligolw_cell_type type)
{
	char *dst = NULL;

	switch(type) {
	case ligolw_cell_type_char_s:
	case ligolw_cell_type_char_v:
	case ligolw_cell_type_ilwdchar:
	case ligolw_cell_type_ilwdchar_u:
	case ligolw_cell_type_lstring:
		if(cell.as_string) {
			int n_escapes;
			char *i;
			const char *j;
			for(n_escapes = 0, j = cell.as_string; *j; j++)
				n_escapes += (*j == '"') || (*j == '\\');
			i = dst = malloc((j - cell.as_string) + n_escapes + 3);
			*i++ = '"';
			for(j = cell.as_string; *j; *i++ = *j++)
				if((*j == '"') || (*j == '\\'))
					*i++ = '\\';
			*i++ = '"';
			*i = 0;
		} else
			dst = strdup("");
		break;

	case ligolw_cell_type_blob:
		if(cell.as_blob.data) {
			char *c;
			base64_encodestate b64state;
			base64_init_encodestate(&b64state);
			c = dst = malloc(2 * cell.as_blob.len /*FIXME: base64_encode_length(n, &b64state)*/ + 3);
			*c++ = '"';
			c += base64_encode_block(cell.as_blob.data, cell.as_blob.len, c, &b64state);
			c += base64_encode_blockend(c, &b64state);
			*c++ = '"';
			*c = 0;
		} else
			dst = strdup("");
		break;

	case ligolw_cell_type_int_2s:
	case ligolw_cell_type_int_4s:
	case ligolw_cell_type_int_8s:
		asprintf(&dst, "%lld", (long long) cell.as_int);
		break;

	case ligolw_cell_type_int_2u:
	case ligolw_cell_type_int_4u:
	case ligolw_cell_type_int_8u:
		asprintf(&dst, "%llu", (unsigned long long) cell.as_uint);
		break;

	case ligolw_cell_type_real_4:
		asprintf(&dst, "%.7g", cell.as_double);
		break;

	case ligolw_cell_type_real_8:
		asprintf(&dst, "%.16g", cell.as_double);
		break;

	case ligolw_cell_type_complex_8: {
		double complex x = cell.as_double_complex;
		asprintf(&dst, "%.7g+i%.7g", creal(x), cimag(x));
		break;
	}

	case ligolw_cell_type_complex_16: {
		double complex x = cell.as_double_complex;
		asprintf(&dst, "%.16g+i%.16g", creal(x), cimag(x));
		break;
	}

	default:
		/* unrecognized type */
		return NULL;
	}

	return dst;
}


/*
 * Assign a union ligolw_cell's contents to a C variable.  dest must point
 * to the location of a C type maching the type of the cell's contents.
 * Returns < 0 on failure, or the size of the data in bytes on success.
 * For blob the size of the allocated data is returned, for strings the
 * size of the pointer is returned.
 *
 * This function takes ownership of the ligolw_cell's data, and transfers
 * that ownership to the C variable.  For numeric types this has no
 * meaning, but for pointer types, strings and blobs, after this function
 * call the calling code is required to free() the C pointer destination
 * when finished with it.  To reduce the risk of errors, the cell's data
 * pointer is set to NULL.  It is safe for calling code to call free() on
 * the cell's pointer, but doing so has no effect on allocated memory.
 */


int ligolw_cell_to_c(union ligolw_cell *cell, enum ligolw_cell_type type, void *dest)
{
	switch(type) {
	case ligolw_cell_type_char_s:
	case ligolw_cell_type_char_v:
	case ligolw_cell_type_ilwdchar:
	case ligolw_cell_type_ilwdchar_u:
	case ligolw_cell_type_lstring:
		*(char **) dest = cell->as_string;
		cell->as_string = NULL;
		return sizeof(char *);

	case ligolw_cell_type_blob:
		*(void **) dest = cell->as_blob.data;
		cell->as_blob.data = NULL;
		return cell->as_blob.len;

	case ligolw_cell_type_int_2s:
		*(int16_t *) dest = cell->as_int;
		return 2;

	case ligolw_cell_type_int_2u:
		*(uint16_t *) dest = cell->as_uint;
		return 2;

	case ligolw_cell_type_int_4s:
		*(int32_t *) dest = cell->as_int;
		return 4;

	case ligolw_cell_type_int_4u:
		*(uint32_t *) dest = cell->as_uint;
		return 4;

	case ligolw_cell_type_int_8s:
		*(int64_t *) dest = cell->as_int;
		return 8;

	case ligolw_cell_type_int_8u:
		*(uint64_t *) dest = cell->as_uint;
		return 8;

	case ligolw_cell_type_real_4:
		*(float *) dest = cell->as_double;
		return 4;

	case ligolw_cell_type_real_8:
		*(double *) dest = cell->as_double;
		return 8;

	case ligolw_cell_type_complex_8:
		*(float complex *) dest = cell->as_double_complex;
		return 8;

	case ligolw_cell_type_complex_16:
		*(double complex *) dest = cell->as_double_complex;
		return 16;

	default:
		/* unrecognized type */
		return -1;
	}
}
