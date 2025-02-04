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
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
#include <libezligolw/ezligolw.h>


/*
 * Extract the meaningful portion of a Name attribute.  Returns a pointer
 * to the last colon-delimited substring before an optional ":suffix".
 */


const char *ligolw_strip_name(const char *Name, const char *suffix)
{
	if(!Name)
		return NULL;

	char buff[strlen(Name) + 1];
	char *pos = buff;
	char *start;

	strcpy(buff, Name);

	if(suffix) {
		int n = strlen(suffix);
		do
			start = strsep(&pos, ":");
		while(pos && strncmp(pos, suffix, n));
	} else {
		do
			start = strsep(&pos, ":");
		while(pos);
	}

	return Name + (start - buff);
}


/*
 * search for and return the first element of the given type in the XML
 * tree rooted at the given element.  if name is not NULL, then the element
 * must have a Name attribute and it must equal the given value.
 *
 * Example:
 *
 *	for(
 *		elem = ligolw_elem_iter(root, "LIGO_LW", "REAL8FrequencySeries");
 *		elem;
 *		elem = ligolw_elem_next(elem, "LIGO_LW", "REAL8FrequencySeries")
 *	) {
 *		...
 *	}
 */


ezxml_t ligolw_elem_iter(ezxml_t elem, const char *type, const char *name)
{
	int n = name ? strlen(name) : 0;

	for(elem = ezxml_child(elem, type); elem; elem = elem->next)
		if(!name || !strncmp(ligolw_strip_name(ezxml_attr(elem, "Name"), NULL), name, n))
			break;
	return elem;
}


ezxml_t ligolw_elem_next(ezxml_t elem, const char *type, const char *name)
{
	int n = name ? strlen(name) : 0;

	for(elem = elem ? elem->next : NULL; elem; elem = elem->next)
		if(!name || !strncmp(ligolw_strip_name(ezxml_attr(elem, "Name"), NULL), name, n))
			break;
	return elem;
}
