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
 * Parse an ezxml_t Param element.
 */


const char *ligolw_param_parse(ezxml_t elem, const char **type)
{
	*type = ezxml_attr(elem, "Type");
	return elem->txt;
}


/*
 * Find an ezxml_t Param element in a document.
 */


ezxml_t ligolw_param_get(ezxml_t xmldoc, const char *name)
{
	ezxml_t elem;

	for(elem = ezxml_child(xmldoc, "Param"); elem; elem = elem->next)
		if(!strcmp(ezxml_attr(elem, "Name"), name))
			break;

	return elem;
}
