/*
 * Copyright (C) 2025  Kipp Cannon
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
#include <lal/Date.h>
#include <lal/LALDict.h>
#include <lal/LALMalloc.h>
#include <lal/FrequencySeries.h>
#include <lal/LALList.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


/*
 * Demonstration of the implementation of an XLAL-style PSD reading
 * function for LAL-based programs.
 */


/*
 * iterate over LIGO_LW elements rooted at elem that contain
 * REAL8FrequencySeries objects.
 */


static ezxml_t ligolw_REAL8FrequencySeries_iter(ezxml_t elem)
{
	return ligolw_elem_iter(elem, "LIGO_LW", "REAL8FrequencySeries");
}


static ezxml_t ligolw_REAL8FrequencySeries_next(ezxml_t elem)
{
	return ligolw_elem_next(elem, "LIGO_LW", "REAL8FrequencySeries");
}


/*
 * Returns a LALDict whose keys are the instrument names and whose values
 * are BLOBValue's containing REAL8FrequencySeries objects.  This function
 * only works properly on files that contain a PSD dictionary and no other
 * REAL8FrequencySeries objects.  It should work if other
 * REAL8FrequencySeries objects are preset as long as the PSD dictionary
 * preceeds them in the file, and they aren't at the same depth in the
 * element tree.  These are not requirements of the file format, this could
 * easily break on what should be considered properly constructed input;
 * these requirement reflect actual use at the time of writing and my own
 * laziness to do a better job.
 *
 * Example:
 *
 * LALDict *psds = XLALPSDsFromLIGOLw("psds.xml");
 * REAL8FrequencySeries *psd_L1 = XLALDictPopBLOBValue(psds, "L1");
 * ...
 * XLALDestroyREAL8FrequencySeries(psd_L1);
 *
 * NOTES:  if the frequency series is popped out of the dictionary, then
 * the calling code owns the reference and must free the frequency series
 * when done.  If only a look-up operation is performed then the dictionary
 * still owns the pointer and the calling code must not free it.
 */


LALDict *XLALPSDsFromLIGOLw(
	const char *filename
)
{
	ezxml_t xmldoc;
	ezxml_t elem;
	LALDict *psds = XLALCreateDict();

	if(!psds)
		XLAL_ERROR_NULL(XLAL_EFUNC);

	/* parse the document */
	xmldoc = ezxml_parse_gzfile(filename);
	if(!xmldoc) {
		XLAL_PRINT_ERROR("error parsing \"%s\"", filename);
		XLALDestroyDict(psds);
		XLAL_ERROR_NULL(XLAL_EIO);
	}

	/* loop over REAL8FrequencySeries objects at the same depth as
	 * whichever one is found first */
	for(elem = ligolw_REAL8FrequencySeries_iter(xmldoc); elem; elem = ligolw_REAL8FrequencySeries_next(elem)) {
		REAL8FrequencySeries *series = ligolw_REAL8FrequencySeries_parse(elem);
		const char *instrument;
		if(!series || ligolw_param_get_as_c(ligolw_param_get(elem, "instrument"), &instrument, ligolw_cell_type_lstring) < 0) {
			XLAL_PRINT_ERROR("failure parsing PSD");
			XLALDestroyDict(psds);
			XLAL_ERROR_NULL(XLAL_EDATA);
		}
		XLALDictInsertBLOBValue(psds, instrument, series, sizeof(*series));
	}

	/* clean up */
	ezxml_free(xmldoc);

	/* success */
	return psds;
}


/*
 * Example use.
 */


int main(int argc, char *argv[])
{
	LALDict *psds = XLALPSDsFromLIGOLw("H1L1V1_O2REFERENCE_psd.xml.gz");
	LALList *instruments = psds ? XLALDictKeys(psds) : NULL;
	LALListItem *item;

	while((item = XLALListPop(instruments))) {
		const char *instrument = XLALListItemGetStringValue(item);
 		REAL8FrequencySeries *psd = XLALDictPopBLOBValue(psds, instrument);

		fprintf(stderr, "found PSD for %s:\n", instrument);
		fprintf(stderr, "\tname = \"%s\"\n", psd->name);
		{
		char *s = XLALGPSToStr(NULL, &psd->epoch);
		fprintf(stderr, "\tepoch = %s\n", s);
		XLALFree(s);
		}
		fprintf(stderr, "\tf0 = %g\n", psd->f0);
		fprintf(stderr, "\tdeltaF = %g\n", psd->deltaF);
		for(int i = 0; i < 4; i++)
			fprintf(stderr, "\tdata[%d] = %g\n", i, psd->data->data[i]);
		fprintf(stderr, "\t...\n");
		for(int i = psd->data->length - 4; i < psd->data->length; i++)
			fprintf(stderr, "\tdata[%d] = %g\n", i, psd->data->data[i]);
		XLALFree(item);
		XLALDestroyREAL8FrequencySeries(psd);
	}

	return 0;
}
