/*
 * Copyright (C) 2025  Kipp Cannon
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
 * Parse an ezxml_t Param element.  The value is returned as a union
 * ligolw_cell object, and the type of the data is stored in the location
 * pointed to by type.
 */


union ligolw_cell ligolw_param_parse(ezxml_t elem, enum ligolw_cell_type *type)
{
	const char *type_name = ezxml_attr(elem, "Type");
	union ligolw_cell value;

	if(!type_name)
		/* default */
		type_name = "lstring";

	*type = ligolw_type_name_to_enum(type_name);
	if(elem)
		ligolw_cell_from_txt(&value, *type, elem->txt);

	return value;
}


/*
 * Find an ezxml_t Param element in a document.  If name is NULL the first
 * element found is reported, otherwise the elment's Name attribute must
 * match name (ignoring an optional :... suffix).
 */


ezxml_t ligolw_param_get(ezxml_t elem, const char *name)
{
	return ligolw_elem_iter(elem, "Param", name);
}


/*
 * Utility for common case of retrieving a parameter value whose type is
 * known.  The value, whose type must be type, is written to the address in
 * dst, which must be large enough to store the value.  The return value is
 * 0 on success, or != 0 on failure, for example if the requested type does
 * not match the Param element's type.
 */


int ligolw_param_get_as_c(ezxml_t elem, void *dst, enum ligolw_cell_type type)
{
	enum ligolw_cell_type param_type;
	union ligolw_cell cell;

	/* this simplifies error checking in calling code */
	if(!elem)
		return -1;

	cell = ligolw_param_parse(elem, &param_type);

	if(param_type != type)
		return -1;

	return ligolw_cell_to_c(&cell, param_type, dst);
}
