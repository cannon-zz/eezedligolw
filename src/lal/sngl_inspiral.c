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
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


int ligolw_sngl_inspiral_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	SnglInspiralTable **head = data;
	SnglInspiralTable *new = LALCalloc(1, sizeof(*new));	/* ugh, lal */
	struct ligolw_unpacking_spec spec[] = {
		{"process:process_id", &new->process_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"event_id", &new->event_id, NULL, ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mass1", &new->mass1, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mass2", &new->mass2, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mtotal", &new->mtotal, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"mchirp", &new->mchirp, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"eta", &new->eta, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1x", &new->spin1x, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1y", &new->spin1y, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin1z", &new->spin1z, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2x", &new->spin2x, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2y", &new->spin2y, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"spin2z", &new->spin2z, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"chi", &new->chi, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"f_final", &new->f_final, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"template_duration", &new->template_duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"ttotal", &new->ttotal, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"search", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"ifo", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"channel", NULL, NULL, ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"sigmasq", &new->sigmasq, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"snr", &new->snr, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"coa_phase", &new->coa_phase, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"eff_distance", &new->eff_distance, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"amplitude", &new->amplitude, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"end_time", &new->end.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"end_time_ns", &new->end.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"end_time_gmst", &new->end_time_gmst, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"impulse_time", &new->impulse_time.gpsSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"impulse_time_ns", &new->impulse_time.gpsNanoSeconds, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bank_chisq", &new->bank_chisq, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"bank_chisq_dof", &new->bank_chisq_dof, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"chisq", &new->chisq, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"chisq_dof", &new->chisq_dof, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"cont_chisq", &new->cont_chisq, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"cont_chisq_dof", &new->cont_chisq_dof, NULL, ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"event_duration", &new->event_duration, NULL, ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"rsqveto_duration", &new->rsqveto_duration, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha", &new->alpha, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha1", &new->alpha1, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha2", &new->alpha2, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha3", &new->alpha3, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha4", &new->alpha4, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha5", &new->alpha5, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"alpha6", &new->alpha6, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"beta", &new->beta, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"kappa", &new->kappa, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"tau0", &new->tau0, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"tau2", &new->tau2, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"tau3", &new->tau3, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"tau4", &new->tau4, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"tau5", &new->tau5, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"psi0", &new->psi0, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"psi3", &new->psi3, NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma0", &new->Gamma[0], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma1", &new->Gamma[1], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma2", &new->Gamma[2], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma3", &new->Gamma[3], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma4", &new->Gamma[4], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma5", &new->Gamma[5], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma6", &new->Gamma[6], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma7", &new->Gamma[7], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma8", &new->Gamma[8], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{"Gamma9", &new->Gamma[9], NULL, ligolw_cell_type_real_4, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, NULL, -1, 0}
	};

	/* check for memory allocation failure.  remember to clean up row's
	 * memory. */
	if(!new) {
		XLALPrintError("memory allocation failure\n");
		goto error;
	}

	/* unpack.  have to do the strings manually because they get copied
	 * by value rather than reference.  */
	if(ligolw_table_unpack_row(table, *row, spec))
		goto error;

	/* do this after unpack_row() to let it confirm the columns are
	 * present and have the correct type.  shouldn't need to check for
	 * errors here */
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "search", NULL), new->search, LIGOMETA_SEARCH_MAX - 1);
	ligolw_cell_string_copy(ligolw_row_get_cell(row, "ifo", NULL), new->ifo, LIGOMETA_IFO_MAX - 1);
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
