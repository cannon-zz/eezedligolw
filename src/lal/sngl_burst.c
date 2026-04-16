/*
 * Copyright (C) 2019,2026  Kipp Cannon
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <lal/LALMalloc.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/XLALError.h>
#include <lal/SnglBurstUtils.h>
#include <ezligolw/lal.h>


int ligolw_sngl_burst_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	SnglBurst **head = data;
	SnglBurst *new = LALCalloc(1, sizeof(*new));
	struct ligolw_unpacking_spec spec[] = {
		{"process:process_id", &new->process_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"event_id", &new->event_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"ifo", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"search", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"channel", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"start_time", &new->start_time.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"start_time_ns", &new->start_time.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"peak_time", &new->peak_time.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"peak_time_ns", &new->peak_time.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"duration", &new->duration, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"central_freq", &new->central_freq, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bandwidth", &new->bandwidth, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"amplitude", &new->amplitude, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"snr", &new->snr, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"confidence", &new->confidence, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"chisq", &new->chisq, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"chisq_dof", &new->chisq_dof, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};

	/* check for memory allocation failure.  remember to clean up row's
	 * memory. */
	if(!new) {
		XLALPrintError("memory allocation failure\n");
		goto error;
	}

	/* unpack.  have to do the strings manually because they get copied
	 * by value rather than reference. */
	if(ligolw_table_unpack_row(table, *row, spec))
		goto error;

	/* do this after unpack_row() to let it confirm the columns are
	 * present and have the correct type.  shouldn't need to check for
	 * errors here */
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "ifo", NULL), new->ifo, LIGOMETA_IFO_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "search", NULL), new->search, LIGOMETA_SEARCH_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "channel", NULL), new->channel, LIGOMETA_CHANNEL_MAX - 1);

	/* add new object to head of linked list */
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
