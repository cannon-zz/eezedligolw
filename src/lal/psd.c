/*
 * Copyright (C) 2025,2026  Kipp Cannon
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


#include <lal/LALDict.h>
#include <lal/FrequencySeries.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


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
 * Searches for a PSD dictionary at or below elem.  A PSD dictionary is a
 * sequence of LIGO_LW elements each encoding a REAL8FrequencySeries
 * object, each having an aditional Param element named "instrument".
 * Decodes the REAL8FrequencySeries objects, and returns a LALDict whose
 * keys are the instrument names and whose values are BLOBValue's
 * containing REAL8FrequencySeries objects.  Returns NULL on failure.
 *
 * Example:
 *
 * LALDict *psds = ligolw_PSDs(xmldoc);
 * REAL8FrequencySeries *psd_L1 = XLALDictPopBLOBValue(psds, "L1");
 * ...
 * XLALDestroyREAL8FrequencySeries(psd_L1);
 *
 * NOTES:  if the frequency series is popped out of the dictionary, then
 * the calling code owns the reference and must free the frequency series
 * when done.  If only a look-up operation is performed then the dictionary
 * still owns the pointer and the calling code must not free it.
 */


LALDict *ligolw_PSDs(
	ezxml_t elem
)
{
	LALDict *psds = XLALCreateDict();

	if(!psds)
		XLAL_ERROR_NULL(XLAL_EFUNC);

	for(elem = ligolw_REAL8FrequencySeries_iter(elem); elem; elem = ligolw_REAL8FrequencySeries_next(elem)) {
		REAL8FrequencySeries *series = ligolw_REAL8FrequencySeries_parse(elem);
		char *instrument;
		if(!series || ligolw_param_get_as_c(ligolw_param_get(elem, "instrument"), &instrument, ligolw_cell_type_lstring) < 0) {
			XLAL_PRINT_ERROR("failure parsing PSD");
			XLALDestroyDict(psds);
			XLAL_ERROR_NULL(XLAL_EDATA);
		}
		XLALDictInsertBLOBValue(psds, instrument, series, sizeof(*series));
		free(instrument);
	}

	/* success */
	return psds;
}


/*
 * Wrapper around ligolw_PSDs() that reads a LIGO Light-Weight XML file,
 * and returns a LALDict of the PSDs contained therein.  This function only
 * works properly on files that contain a PSD dictionary and no other
 * REAL8FrequencySeries objects.  It should work if other
 * REAL8FrequencySeries objects are preset as long as the PSD dictionary
 * preceeds them in the file, and they aren't at the same depth in the
 * element tree.  These are not requirements of the file format, this could
 * easily break on what should be considered properly constructed input;
 * these requirements reflect actual use at the time of writing and my own
 * laziness to do a better job.
 *
 * Example:
 *
 * LALDict *psds = ligolw_PSDsFromFile("psds.xml");
 */


LALDict *ligolw_PSDsFromFile(
	const char *filename
)
{
	ezxml_t xmldoc;
	LALDict *psds;

	xmldoc = ezxml_parse_gzfile(filename);
	if(!xmldoc) {
		XLAL_PRINT_ERROR("error parsing \"%s\"", filename);
		XLAL_ERROR_NULL(XLAL_EIO);
	}

	psds = ligolw_PSDs(xmldoc);

	ezxml_free(xmldoc);
	return psds;
}
