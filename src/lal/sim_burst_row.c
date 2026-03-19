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


int ligolw_sim_burst_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	int result_code;
	SimBurst **head = data;
	SimBurst *new = XLALCreateSimBurst();
	struct ligolw_unpacking_spec sim_burst_basic[] = {
		{"process:process_id", &new->process_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"waveform", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"ra", &new->ra, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"dec", &new->dec, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"psi", &new->psi, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"time_geocent_gps", &new->time_geocent_gps.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"time_geocent_gps_ns", &new->time_geocent_gps.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"time_geocent_gmst", &new->time_geocent_gmst, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"time_slide:time_slide_id", &new->time_slide_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"simulation_id", &new->simulation_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_StringCusp[] = {
		{"duration", &new->duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"frequency", &new->frequency, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"amplitude", &new->amplitude, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_SineGaussian[] = {
		{"duration", &new->duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"frequency", &new->frequency, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bandwidth", &new->bandwidth, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"q", &new->q, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"pol_ellipse_angle", &new->pol_ellipse_angle, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"pol_ellipse_e", &new->pol_ellipse_e, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"hrss", &new->hrss, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_Gaussian[] = {
		{"duration", &new->duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"hrss", &new->hrss, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_BTLWNB[] = {
		{"duration", &new->duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"frequency", &new->frequency, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bandwidth", &new->bandwidth, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"pol_ellipse_angle", &new->pol_ellipse_angle, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"pol_ellipse_e", &new->pol_ellipse_e, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"egw_over_rsquared", &new->egw_over_rsquared, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"waveform_number", &new->waveform_number, NULL, ligolw_cell_type_int_8u, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_Impulse[] = {
		{"amplitude", &new->amplitude, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};

	/* check for memory allocation failure */
	if(!new) {
		XLALPrintError("memory allocation failure\n");
		goto error;
	}

	/* unpack the base columns.  have to do the strings manually
	 * because they get copied by value rather than reference. */
	result_code = ligolw_lal_table_unpack_row(table, *row, sim_burst_basic);
	if(result_code)
		goto error;

	/* do this after unpack_row() to let it confirm the column is
	 * present and has the correct type.  shouldn't need to check for
	 * errors here */
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "waveform", NULL), new->waveform, LIGOMETA_WAVEFORM_MAX - 1);

	/* unpack additional columns depending on the waveform */
	if(!strcmp(new->waveform, "StringCusp"))
		result_code = ligolw_table_unpack_row(table, *row, sim_burst_StringCusp);
	else if(!strcmp(new->waveform, "SineGaussian") || !strcmp(new->waveform, "SineGaussianF"))
		result_code = ligolw_table_unpack_row(table, *row, sim_burst_SineGaussian);
	else if(!strcmp(new->waveform, "Gaussian"))
		result_code = ligolw_table_unpack_row(table, *row, sim_burst_Gaussian);
	else if(!strcmp(new->waveform, "BTLWNB"))
		result_code = ligolw_table_unpack_row(table, *row, sim_burst_BTLWNB);
	else if(!strcmp(new->waveform, "Impulse"))
		result_code = ligolw_table_unpack_row(table, *row, sim_burst_Impulse);
	else {
		/* unrecognized waveform */
		XLALPrintError("failure parsing row: unrecognized waveform \"%s\"\n", new->waveform);
		goto error;
	}
	if(result_code)
		goto error;

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
