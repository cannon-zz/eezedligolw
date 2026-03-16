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
#include <ezligolw/ezligolw.h>


/*
 * Demonstration of the implementation of an XLAL-style (near) drop-in
 * replacement of LALSimBurstTableFromLIGOLw() built on top of ezligolw.
 * For clarity, the LAL code's ability to filter the input trigger list by
 * GPS time has not been implemented here.
 */


static int sim_burst_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *data)
{
	int result_code;
	SimBurst **head = data;
	SimBurst *new = XLALCreateSimBurst();
	union ligolw_cell *str;
	struct ligolw_unpacking_spec *spec;
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

	/* do this after ligolw_table_unpack_row() to let it confirm the
	 * column is present and has the correct type.  we've confirmed the
	 * column exists and has the correct type, so shouldn't need to
	 * check for errors */
	str = ligolw_row_get_cell(row, "waveform", NULL);
	strncpy(new->waveform, str->as_string, LIGOMETA_WAVEFORM_MAX - 1);
	new->waveform[LIGOMETA_WAVEFORM_MAX-1] = '\0';
	/* we are treated as taking ownership of this, so we need to free
	 * it */
	free(str->as_string);
	str->as_string = NULL;

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

	/* add new sim to head of linked list.  the linked list's elements
	 * are reversed with respect to the file. */
	new->next = *head;
	*head = new;

	/* success */
	ligolw_table_free_row_data(table, row);
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
	ligolw_table_free_row_data(table, row);
	free(row);
	return -1;
}


static int XLALTableFromLIGOLw(
	void *head,
	const char *filename,
	const char *table_name,
	int row_callback(struct ligolw_table *, struct ligolw_table_row *, void *)
)
{
	ezxml_t xmldoc;
	ezxml_t elem;
	struct ligolw_table *table;

	/* initialize the linked list */
	*(void **) head = NULL;

	/* parse the document */
	xmldoc = ezxml_parse_gzfile(filename);
	if(!xmldoc) {
		XLALPrintError("error parsing %s\n", filename);
		XLAL_ERROR(XLAL_EIO);
	}

	/* find the table */
	elem = ligolw_table_get(xmldoc, table_name);
	if(!elem) {
		XLALPrintError("unable to locate %s table\n", table_name);
		ezxml_free(xmldoc);
		XLAL_ERROR(XLAL_EDATA);
	}

	/* convert the rows to a LAL-style linked list */
	table = ligolw_table_parse(elem, row_callback, head);
	if(!table) {
		XLALPrintError("failure parsing %s table\n", table_name);
		ezxml_free(xmldoc);
		XLAL_ERROR(XLAL_EDATA);
	}

	/* clean up */
	ligolw_table_free(table);
	ezxml_free(xmldoc);

	/* success */
	return 0;
}


int XLALSimBurstTableFromLIGOLw(
	SimBurst **head,
	const char *filename
)
{
	return XLALTableFromLIGOLw(head, filename, "sim_burst", sim_burst_row_callback);
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

	XLALSimBurstTableFromLIGOLw(&sims, "HL-INJECTIONS_PLAYGROUND-793154935-2524278.xml.gz");

	write(sims, "output.xml");

	XLALDestroySimBurstTable(sims);

	return 0;
}
