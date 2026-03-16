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


#include <ezligolw/ezligolw.h>


enum ligolw_stream_encoding {
	ligolw_stream_enc_Text		= 0x01,
	ligolw_stream_enc_Binary	= 0x02,
	ligolw_stream_enc_uuencode	= 0x04,
	ligolw_stream_enc_base64	= 0x08,
	ligolw_stream_enc_BigEndian	= 0x10,
	ligolw_stream_enc_LittleEndian	= 0x20,
	ligolw_stream_enc_Delimiter	= 0x40,
	/* the only supported encodings: */
	ligolw_stream_enc_text		= 0x41,	/* delimted text encoding */
	ligolw_stream_enc_b64be		= 0x18, /* base64 big endian */
	ligolw_stream_enc_b64le		= 0x28  /* base64 little endian */
};

char ligolw_stream_delimiter(ezxml_t);
enum ligolw_stream_encoding ligolw_stream_check_encoding(ezxml_t stream);
void ligolw_stream_next_token(char **, char **, char **, char);
