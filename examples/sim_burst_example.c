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
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


/*
 * Demonstration of LAL tabular data loading.
 */


static void write(const char *filename, const SimBurst *sims, const TimeSlide *tisls)
{
	LIGOLwXMLStream *xml = XLALOpenLIGOLwXMLFile(filename);
	XLALWriteLIGOLwXMLSimBurstTable(xml, sims);
	XLALWriteLIGOLwXMLTimeSlideTable(xml, tisls);
	XLALCloseLIGOLwXMLFile(xml);
}



int main(int argc, char *argv[])
{
	const char *filename = "HL-INJECTIONS_PLAYGROUND-793154935-2524278.xml.gz";
	ezxml_t xmldoc;
	SimBurst *sims;
	TimeSlide *tisls;

	xmldoc = ezxml_parse_gzfile(filename);
	if(!xmldoc) {
		XLALPrintError("error parsing %s\n", filename);
		return 1;
	}

	sims = ligolw_lal_table_get(xmldoc, "sim_burst", NULL);
	tisls = ligolw_lal_table_get(xmldoc, "time_slide", NULL);

	/* all required data has been copied */
	ezxml_free(xmldoc);

	if(!sims || !tisls) {
		XLALPrintError("error parsing %s\n", filename);
		return 1;
	}

	write("output.xml", sims, tisls);

	XLALDestroySimBurstTable(sims);
	XLALDestroyTimeSlideTable(tisls);

	return 0;
}
