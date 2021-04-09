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
 * Extract the meaningful portion of a Name attribute.  Returns a pointer
 * to the last colon-delimited substring before an optional ":suffix".
 */


const char *ligolw_strip_name(const char *Name, const char *suffix)
{
	char buff[strlen(Name) + 1];
	int n = strlen(suffix);
	char *pos = buff;
	char *start;

	strcpy(buff, Name);

	do
		start = strsep(&pos, ":");
	while(pos && strncmp(pos, suffix, n));

	return Name + (start - buff);
}
