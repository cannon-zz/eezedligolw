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


int ligolw_sim_inspiral_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	int result_code;
	SimInspiralTable **head = data;
	SimInspiralTable *new = LALCalloc(1, sizeof(*new));	/* ugh, lal */
	struct ligolw_unpacking_spec spec[] = {
		{"process:process_id", &new->process_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"waveform", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"geocent_end_time", &new->geocent_end_time.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"geocent_end_time_ns", &new->geocent_end_time.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		/* don't load detector end times:  they're stupid */
		{"source", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mass1", &new->mass1, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mass2", &new->mass2, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mchirp", &new->mchirp, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"eta", &new->eta, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"distance", &new->distance, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"longitude", &new->longitude, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"latitude", &new->latitude, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"inclination", &new->inclination, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"coa_phase", &new->coa_phase, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"polarization", &new->polarization, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"psi0", &new->psi0, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"psi3", &new->psi3, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha", &new->alpha, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha1", &new->alpha1, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha2", &new->alpha2, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha3", &new->alpha3, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha4", &new->alpha4, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha5", &new->alpha5, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha6", &new->alpha6, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"beta", &new->beta, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1x", &new->spin1x, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1y", &new->spin1y, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1z", &new->spin1z, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2x", &new->spin2x, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2y", &new->spin2y, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2z", &new->spin2z, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"theta0", &new->theta0, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"phi0", &new->phi0, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"f_lower", &new->f_lower, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"f_final", &new->f_final, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		/* don't load effective distances:  they're stupid */
		{"numrel_mode_min", &new->numrel_mode_min, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"numrel_mode_max", &new->numrel_mode_max, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"numrel_data", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"amp_order", &new->amp_order, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"taper", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bandpass", &new->bandpass, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"simulation_id", &new->simulation_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
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
	result_code = ligolw_table_unpack_row(table, *row, spec);
	if(result_code)
		goto error;

	/* do this after ligolw_table_unpack_row() to let it confirm the
	 * columns are present and ahve the correct type.  shouldn't need
	 * to check for errors */
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "waveform", NULL), new->waveform, LIGOMETA_WAVEFORM_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "source", NULL), new->source, LIGOMETA_SOURCE_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "numrel_data", NULL), new->numrel_data, LIGOMETA_STRING_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "taper", NULL), new->taper, LIGOMETA_INSPIRALTAPER_MAX - 1);

	/* add new sim to head of linked list.  yes, this means the table's
	 * rows get reversed.  so what. */
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
