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
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <./stream.h>
#include <ezligolw/ezligolw.h>


#define QUOTE_CHAR	'"'
#define ESCAPE_CHAR	'\\'


/*
 * Report the Stream element's delimiter character.  Returns the default
 * delimiter if one is not set.  Returns < 0 if the delimiter is invalid.
 * If the XML element is not a Stream element the result is undefined.
 */


char ligolw_stream_delimiter(ezxml_t stream)
{
	const char *attr = ezxml_attr(stream, "Delimiter");
	/* default is ',' */
	if(!attr)
		return ',';
	/* delimiter must be single printable character or space */
	if(strlen(attr) > 1 || !isprint(*attr))
		return -1;
	return *attr;
}


/*
 * Check that the Stream element's encoding related attributes are set to
 * allowed values, and report the encoding type enum.  Returns < 0 on
 * errors, of if the encoding is invalid, or if the encoding is valid but
 * not supported.  If the encoding is valid and supported, the return value
 * will be one of the three constants:  ligolw_stream_enc_text (delimited
 * text), ligolw_stream_enc_b64be (base64 encoded big-endian data), or
 * ligolw_stream_enc_b64le (base64 encoded little-endian data).
 */


enum ligolw_stream_encoding ligolw_stream_check_encoding(ezxml_t stream)
{
	const char *attr;
	char *encoding;
	char *state;
	int encbits;
	static const struct enckeywords {
		const char *keyword;
		enum ligolw_stream_encoding encbit;
	} keywords[] = {
		{"Text", ligolw_stream_enc_Text},
		{"Binary", ligolw_stream_enc_Binary},
		{"uuencode", ligolw_stream_enc_uuencode},
		{"base64", ligolw_stream_enc_base64},
		{"BigEndian", ligolw_stream_enc_BigEndian},
		{"LittleEndian", ligolw_stream_enc_LittleEndian},
		{"Delimiter", ligolw_stream_enc_Delimiter},
		{NULL, 0}
	};

	/*
	 * confirm that Type="Local" (also the default value)
	 */

	attr = ezxml_attr(stream, "Type");
	if(!attr || !strcmp(attr, "Local")) {
		/* is default or is "Local" */
	} else if(!strcmp(attr, "Remote")) {
		/* Type attribute valid, but not suported */
		return -1;
	} else {
		/* Type attribute set to invalid value */
		return -1;
	}

	/*
	 * confirm that Content is valid if set, but ignore it
	 */

	attr = ezxml_attr(stream, "Content");
	if(attr && strcmp(attr, "Typed") && strcmp(attr, "Raw") && strcmp(attr, "MIME")) {
		/* Content attribute set to invalid value */
		return -1;
	}

	/*
	 * check for and interpret Encoding.  attribute value is
	 * separated on ',' characters, each piece has leading and trailing
	 * whitespace stripped, and is compared to the list of recognized
	 * encoding keywords.
	 *
	 * the tokenizer is given all the whitespace characters in addition
	 * to the ',' as a lazy way to strip leading and trailing
	 * whitespace, but this alters the behaviour so that, for example,
	 * "aaa, bbb ccc" will be interpreted as the three-keyword sequence
	 * {"aaa", "bbb", "ccc"} instead of, correctly, as {"aaa", "bbb
	 * ccc"}.  I think I don't care:  congradulations, your borken file
	 * tricked the code, and messed up your programme.  sounds like a
	 * you problem.
	 */

	attr = ezxml_attr(stream, "Encoding");
	encoding = strdup(attr ? attr : "Text");
	encbits = 0;
	for(attr = strtok_r(encoding, ", \f\n\r\t\v", &state); attr; attr = strtok_r(NULL, ", \f\n\r\t\v", &state)) {
		const struct enckeywords *keyword;
		for(keyword = keywords; ; keyword++) {
			/* if we get to the end of the array without
			 * recognizing this piece then this Encoding is
			 * invalid */
			if(!keyword->keyword) {
				free(encoding);
				return -1;
			}
			if(!strcmp(attr, keyword->keyword)) {
				encbits |= keyword->encbit;
				break;
			}
		}
	}
	free(encoding);

	/*
	 * we have a valid encoding.  limit the result to one of the
	 * supported encodings
	 */

	switch(encbits) {
	case ligolw_stream_enc_Delimiter:
	case ligolw_stream_enc_Text:
	case ligolw_stream_enc_Text | ligolw_stream_enc_Delimiter:
		encbits = ligolw_stream_enc_text;
		break;

	case ligolw_stream_enc_base64 | ligolw_stream_enc_BigEndian:
	case ligolw_stream_enc_base64 | ligolw_stream_enc_LittleEndian:
		break;

	default:
		/* encoding is not supported */
		encbits = -1;
		break;
	}

	return encbits;
}


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
