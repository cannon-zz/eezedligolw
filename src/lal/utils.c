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


#include <string.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


/*
 * Wrapper around ligolw_table_unpack_row() that reports errors via the
 * XLALPrintError() error reporting mechanism.  Reduces line count in LAL
 * oriented row handlers.
 */


int ligolw_lal_table_unpack_row(struct ligolw_table *table, struct ligolw_table_row row, struct ligolw_unpacking_spec *spec)
{
	int result_code;

	result_code = ligolw_table_unpack_row(table, row, spec);

	if(result_code > 0) {
		/* missing required column */
		XLALPrintError("failure parsing row: missing column \"%s\"\n", spec[result_code - 1].name);
	} else if(result_code < 0) {
		/* column type mismatch */
		XLALPrintError("failure parsing row: incorrect type for column \"%s\"\n", spec[-result_code - 1].name);
	}

	return result_code;
}


/*
 * Wrapper around ligolw_table_get() and ligolw_table_parse() to be used
 * together with a LAL table row unpack function to populate a linked list
 * of LAL table row structures.  If row_callback() is NULL, table_name will
 * be used to select the row callback function supplied by this library
 * corresponding to that table.
 *
 * Returns the address of the head of a linked list of rows, or NULL on
 * error.  NOTE that an empty table will also be reported as NULL.  Errors
 * are reported using the LAL error reporting mechanism, and so the
 * distinction between a failed read and a successful read of an empty
 * table can be checked by consulting xlalErrno.
 */


void *ligolw_lal_table_get(
	ezxml_t elem,
	const char *table_name,
	int row_callback(struct ligolw_table *, struct ligolw_table_row *, void *)
)
{
	struct ligolw_table *table;
	/* empty linked list */
	void *head = NULL;

	/* select row_callback() if neded */
	if(row_callback) {
	} else if(!strcmp(table_name, "sim_burst"))
		row_callback = ligolw_sim_burst_row_callback;
	else if(!strcmp(table_name, "sim_inspiral"))
		row_callback = ligolw_sim_inspiral_row_callback;
	else if(!strcmp(table_name, "sngl_inspiral"))
		row_callback = ligolw_sngl_inspiral_row_callback;
	else if(!strcmp(table_name, "time_slide"))
		row_callback = ligolw_time_slide_row_callback;
	else {
		XLALPrintError("table \"%s\" not recognized\n", table_name);
		XLAL_ERROR_NULL(XLAL_EINVAL);
	}

	/* find the table */
	elem = ligolw_table_get(elem, table_name);
	if(!elem) {
		XLALPrintError("unable to locate \"%s\" table\n", table_name);
		XLAL_ERROR_NULL(XLAL_EDATA);
	}

	/* convert the rows to a LAL-style linked list */
	table = ligolw_table_parse(elem, row_callback, &head);
	if(!table) {
		XLALPrintError("failure parsing \"%s\" table\n", table_name);
		XLAL_ERROR_NULL(XLAL_EDATA);
	}

	/* clean up */
	ligolw_table_free(table);

	/* success */
	return head;
}
