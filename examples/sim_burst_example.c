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


#include <lal/LIGOLwXML.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/SnglBurstUtils.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>


int sim_burst_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);
int time_slide_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);
int sim_insprial_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);


/*
 * Demonstration of the implementation of an XLAL-style (near) drop-in
 * replacement of LALSimBurstTableFromLIGOLw() built on top of ezligolw.
 * For clarity, the LAL code's ability to filter the input trigger list by
 * GPS time has not been implemented here.
 */


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
	SimBurst **sims,
	TimeSlide **tisls,
	const char *filename
)
{
	int failure = false;

	failure |= XLALTableFromLIGOLw(sims, filename, "sim_burst", sim_burst_row_callback);
	failure |= XLALTableFromLIGOLw(tisls, filename, "time_slide", time_slide_row_callback);

	return failure;
}


/*
 * Example use.
 */


static void write(const char *filename, const SimBurst *sims, const TimeSlide *tisls)
{
	LIGOLwXMLStream *xml;

	xml = XLALOpenLIGOLwXMLFile(filename);
	XLALWriteLIGOLwXMLSimBurstTable(xml, sims);
	XLALWriteLIGOLwXMLTimeSlideTable(xml, tisls);
	XLALCloseLIGOLwXMLFile(xml);
}



int main(int argc, char *argv[])
{
	SimBurst *sims;
	TimeSlide *tisls;

	XLALSimBurstTableFromLIGOLw(&sims, &tisls, "HL-INJECTIONS_PLAYGROUND-793154935-2524278.xml.gz");

	write("output.xml", sims, tisls);

	XLALDestroySimBurstTable(sims);
	XLALDestroyTimeSlideTable(tisls);

	return 0;
}
