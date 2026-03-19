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


#include <lal/Date.h>
#include <lal/LALDict.h>
#include <lal/LALMalloc.h>
#include <lal/FrequencySeries.h>
#include <ezligolw/lal.h>


/*
 * Demonstration of the implementation of an XLAL-style PSD reading
 * function for LAL-based programs.
 */


int main(int argc, char *argv[])
{
	LALDict *psds = ligolw_PSDsFromFile("H1L1V1_O2REFERENCE_psd.xml.gz");
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
