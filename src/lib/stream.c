/*
 * Copyright (C) 2007,2026  Kipp Cannon
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
#include <ctype.h>
#include <libezligolw/ezligolw.h>


#define QUOTE_CHAR	'"'
#define ESCAPE_CHAR	'\\'


/*
 * Identify the start and end of the next delimited token in an XML Stream
 * element.  Shared by Table and Array parsing.
 */


void ligolw_stream_next_token(char **txt, char **start, char **end, char delimiter)
{
	char *c;

	/* find the token's start, the first non-white space character */
	for(c = *txt; isspace(*c); c++);

	if(*c == QUOTE_CHAR) {
		/* quoted token */

		/* start is first character after quote charater.  below,
		 * end will be left pointing to the first character after
		 * the token, either the terminating quote character or the
		 * null terminator */

		*start = ++c;

		/* find the first escape sequence, an unescaped quote
		 * character, or the null terminator */
		for(; *c && *c != ESCAPE_CHAR && *c != QUOTE_CHAR; c++);

		if(*c == ESCAPE_CHAR) {
			/* this quoted token contains escaped special
			 * characters.  process the remainder of the token
			 * using escape handling logic */

			char *j = c;

			goto start;
			for(; *j && *j != QUOTE_CHAR; *(c++) = *(j++)) {
				/*
				 * is this character the escape character?
				 */

				if(*j != ESCAPE_CHAR)
					continue;

start:
				/*
				 * check for an unrecognized escape
				 * sequence, or an escape sequence starting
				 * in the last character position.
				 */

				if(!*(++j)) {
					/* FIXME:  report incomplete escape
					 * sequence at end of string error */
					assert(false);
				} else if(*j != ESCAPE_CHAR && *j != QUOTE_CHAR) {
					/* FIXME:  report unrecognized escape
					 * sequence error */
				}
			}
			/* move the final character, the one that
			 * terminated the token */
			*c = *j;

			/* record the end position */
			*end = c;

			/* continue processing from the original position
			 * of the token's end */
			c = j;
		} else
			/* no escaped characters encountered.  record the
			 * end position */
			*end = c;

		/* skip the quote character (might be at end of buffer, at
		 * a null terminator, to only check) */
		if(*c == QUOTE_CHAR)
			c++;
	} else {
		/* unquoted token */
		/* start at first non-white space character */
		*start = c;
		/* end at space or delimiter or '\0' */
		for(; *c && !isspace(*c) && *c != delimiter; c++);
		*end = c;
	}

	/* advance to what should be the delimiter marking the
	 * boundary between this and the next tokens */
	for(; isspace(*c) && *c != delimiter; c++);

	/* next token processing starts after delimiter */
	if(*c == delimiter)
		*txt = c + 1;
	else if(!*c)
		*txt = c;
	else {
		/* FIXME:  expected white space or delimiter following
		 * token, found something else */
		assert(false);
	}
}
