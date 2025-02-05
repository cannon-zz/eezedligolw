/*
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


#include <lal/LIGOLwXML.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/SnglBurstUtils.h>
#include <lal/XLALError.h>
#include <libezligolw/ezligolw.h>


/*
 * Demonstration of the implementation of an XLAL-style drop-in replacement
 * of LALSimBurstTableFromLIGOLw() built on top of ezligolw.  For clarity,
 * the LAL code's ability to filter the input trigger list by GPS time has
 * not been implemented here.
 */


static int sim_burst_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	int result_code;
	SimBurst **head = data;
	SimBurst *new = XLALCreateSimBurst();
	struct ligolw_unpacking_spec *spec;
	struct ligolw_unpacking_spec sim_burst_basic[] = {
		{&new->process_id, "process:process_id", ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, "waveform", ligolw_cell_type_lstring, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->ra, "ra", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->dec, "dec", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->psi, "psi", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->time_geocent_gps.gpsSeconds, "time_geocent_gps", ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->time_geocent_gps.gpsNanoSeconds, "time_geocent_gps_ns", ligolw_cell_type_int_4s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->time_geocent_gmst, "time_geocent_gmst", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->time_slide_id, "time_slide:time_slide_id", ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->simulation_id, "simulation_id", ligolw_cell_type_int_8s, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_StringCusp[] = {
		{&new->duration, "duration", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->frequency, "frequency", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->amplitude, "amplitude", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_SineGaussian[] = {
		{&new->duration, "duration", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->frequency, "frequency", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->bandwidth, "bandwidth", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->q, "q", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->pol_ellipse_angle, "pol_ellipse_angle", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->pol_ellipse_e, "pol_ellipse_e", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->hrss, "hrss", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_Gaussian[] = {
		{&new->duration, "duration", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->hrss, "hrss", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_BTLWNB[] = {
		{&new->duration, "duration", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->frequency, "frequency", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->bandwidth, "bandwidth", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->pol_ellipse_angle, "pol_ellipse_angle", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->pol_ellipse_e, "pol_ellipse_e", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->egw_over_rsquared, "egw_over_rsquared", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{&new->waveform_number, "waveform_number", ligolw_cell_type_int_8u, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};
	struct ligolw_unpacking_spec sim_burst_Impulse[] = {
		{&new->amplitude, "amplitude", ligolw_cell_type_real_8, LIGOLW_COLUMN_FLAGS_REQUIRED},
		{NULL, NULL, -1, 0}
	};

	/* check for memory allocation failure.  remember to clean up row's
	 * memory. */
	if(!new) {
		XLALPrintError("memory allocation failure\n");
		goto error;
	}

	/* unpack the base columns.  have to do the strings manually
	 * because they get copied by value rather than reference. */
	result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_basic);
	if(result_code)
		goto unpackerror;

	/* do this after ligolw_unpacking_row_builder() to let it confirm
	 * the column is present and has the correct type */
	strncpy(new->waveform, ligolw_row_get_cell(row, "waveform", NULL).as_string, LIGOMETA_WAVEFORM_MAX - 1);
	new->waveform[LIGOMETA_WAVEFORM_MAX-1] = '\0';

	/* unpack additional columns depending on the waveform */
	if(!strcmp(new->waveform, "StringCusp"))
		result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_StringCusp);
	else if(!strcmp(new->waveform, "SineGaussian") || !strcmp(new->waveform, "SineGaussianF"))
		result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_SineGaussian);
	else if(!strcmp(new->waveform, "Gaussian"))
		result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_Gaussian);
	else if(!strcmp(new->waveform, "BTLWNB"))
		result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_BTLWNB);
	else if(!strcmp(new->waveform, "Impulse"))
		result_code = ligolw_table_unpack_row(table, *row, spec = sim_burst_Impulse);
	else {
		/* unrecognized waveform */
		XLALPrintError("failure parsing row: unrecognized waveform \"%s\"\n", new->waveform);
		goto error;
	}
	if(result_code)
		goto unpackerror;

	/* add new sim to head of linked list.  yes, this means the table's
	 * rows get reversed.  so what. */
	new->next = *head;
	*head = new;

	/* success */
	free(row->cells);
	free(row);
	return 0;

unpackerror:
	if(result_code > 0) {
		/* missing required column */
		XLALPrintError("failure parsing row: missing column \"%s\"\n", spec[result_code - 1].name);
	} else if(result_code < 0) {
		/* column type mismatch */
		XLALPrintError("failure parsing row: incorrect type for column \"%s\"\n", spec[-result_code - 1].name);
	}
error:
	free(new);
	free(row->cells);
	free(row);
	return -1;
}


int XLALSimBurstTableFromLIGOLw(
	SimBurst **head,
	const char *filename,
	int gps_start,
	int gps_end
)
{
	ezxml_t xmldoc;
	ezxml_t elem;
	struct ligolw_table *table;

	/* initialize the linked list */
	*head = NULL;

	/* parse the document */
	xmldoc = ezxml_parse_gzfile(filename);
	if(!xmldoc) {
		XLALPrintError("error parsing %s\n", filename);
		XLAL_ERROR(XLAL_EIO);
	}

	/* find the sim_burst table */
	elem = ligolw_table_get(xmldoc, "sim_burst");
	if(!elem) {
		XLALPrintError("unable to locate sim_burst table\n");
		ezxml_free(xmldoc);
		XLAL_ERROR(XLAL_EDATA);
	}

	/* convert the rows into a LAL-style linked list */
	table = ligolw_table_parse(elem, sim_burst_row_callback, head);
	if(!table) {
		XLALPrintError("failure parsing sim_burst table\n");
		ezxml_free(xmldoc);
		XLAL_ERROR(XLAL_EDATA);
	}

	/* clean up */
	ligolw_table_free(table);
	ezxml_free(xmldoc);

	/* success */
	return 0;
}


/*
 * Example use.
 */


static void write(const SimBurst *sims, const char *filename)
{
	LIGOLwXMLStream *xml;

	xml = XLALOpenLIGOLwXMLFile(filename);
	XLALWriteLIGOLwXMLSimBurstTable(xml, sims);
	XLALCloseLIGOLwXMLFile(xml);
}



int main(int argc, char *argv[])
{
	SimBurst *sims;

	XLALSimBurstTableFromLIGOLw(&sims, "HL-INJECTIONS_PLAYGROUND-793154935-2524278.xml.gz", 0, 0);

	write(sims, "output.xml");

	return 0;
}
