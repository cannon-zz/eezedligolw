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

#include <ctype.h>
#include <libezligolw/ezligolw.h>


/*
 * Identify the start and end of the next delimited token in an XML Stream
 * element.  Shared by Table and Array parsing.
 */


void ligolw_next_token(char **start, char **end, char **next_start, char delimiter)
{
	char *c;

	/* find the token's start */
	for(c = *start; *c && isspace(*c) && *c != delimiter && *c != '"'; c++);

	/* quoted token */
	if(*c == '"') {
		/* start at first character next to quote charater '"' */
		*start = ++c;
		/* end at '\0' or '"' */
		for(; *c && *c != '"'; c++);
		*end = c;
		/* find the delimiter, this marks the end of current token */
		if(*c == '"')
			c++;
		for(; *c && isspace(*c) && *c != delimiter; c++);
	}
	/* token has zero length */
	else if(!*c || *c == delimiter) {
		/* at the delimiter, this marks the end of current token */
		*start = *end = c;
	}
	/* unquoted token */
	else {
		/* start at first non-white space and non-quote character */
		*start = c;
		/* end at space or delimiter or '\0' */
		for(++c; *c && !isspace(*c) && *c != delimiter; c++);
		*end = c;
		/* find the delimiter, this marks the end of current token */
		for(; *c && isspace(*c) && *c != delimiter; c++);
	}

	/* skip the delimiter and white spaces and go to next start */
	if(*c == delimiter)
		c++;
	for(; *c && isspace(*c) && *c != delimiter; c++);
	*next_start = c;
}
