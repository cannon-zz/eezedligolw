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


#include <string.h>
#include <libezligolw/ezligolw.h>


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
