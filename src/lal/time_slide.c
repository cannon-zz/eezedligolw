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


#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


int ligolw_time_slide_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	int result_code;
	TimeSlide **head = data;
	TimeSlide *new = XLALCreateTimeSlide();
	struct ligolw_unpacking_spec spec[] = {
		{"process:process_id", &new->process_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"time_slide_id", &new->time_slide_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"instrument", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"offset", &new->offset, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};

	/* check for memory allocation failure */
	if(!new) {
		XLALPrintError("memory allocation failure\n");
		goto error;
	}

	/* unpack.  have to do the strings manually because they get copied
	 * by value rather than reference. */
	result_code = ligolw_table_unpack_row(table, *row, spec);
	if(result_code)
		goto error;

	/* do this after unpack_row() to let it confirm the column is
	 * present and has the correct type.  shouldn't need to check for
	 * errors here */
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "instrument", NULL), new->instrument, LIGOMETA_STRING_MAX - 1);

	/* add new sim to head of linked list */
	new->next = *head;
	*head = new;

	/* success */
	ligolw_table_free_row_data(table, row);
	free(row);
	return 0;

error:
	free(new);
	ligolw_table_free_row_data(table, row);
	free(row);
	return -1;
}
