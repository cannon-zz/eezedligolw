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
 * Parse an ezxml_t Time element.  Sets type to the address of a string
 * containing the type's type, and returns the address of the text contents
 * of the element.  Neither are owned by the calling code, they must not be
 * free()'ed.
 */


const char *ligolw_time_parse(ezxml_t elem, const char **type)
{
	/* this simplifies error checking in calling code */
	if(!elem) {
		*type = NULL;
		return NULL;
	}
	*type = ezxml_attr(elem, "Type");
	return elem->txt;
}


/*
 * Find an ezxml_t Time element in a document.  If name is NULL the first
 * element found is reported, otherwise the elment's Name attribute must
 * match name (ignoring an optional :... suffix).
 */


ezxml_t ligolw_time_get(ezxml_t elem, const char *name)
{
	return ligolw_elem_iter(elem, "Time", name);
}
