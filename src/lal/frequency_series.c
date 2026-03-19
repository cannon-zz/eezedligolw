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


#include <string.h>
#include <lal/Date.h>
#include <lal/FrequencySeries.h>
#include <lal/Units.h>
#include <lal/XLALError.h>
#include <ezligolw/ezligolw.h>
#include <ezligolw/lal.h>


/*
 * Construct a LAL REAL8FrequencySeries object from the LIGO_LW element
 * located at elem.  Errors are reported using the XLAL error reporting
 * mechanism.
 */


REAL8FrequencySeries *ligolw_REAL8FrequencySeries_parse(ezxml_t elem)
{
	const char *epoch_type;
	const char *epoch = ligolw_time_parse(ligolw_time_get(elem, "epoch"), &epoch_type);
	LIGOTimeGPS gps;
	struct ligolw_array *array = ligolw_array_parse(ligolw_array_get(elem, NULL));
	char *name;
	enum ligolw_cell_type f0_type;
	double deltaF;
	LALUnit sampleUnits;
	REAL8FrequencySeries *series;
	int i;

	if(!array) {
		XLAL_PRINT_ERROR("Array element tree decode failed");
		return NULL;
	}

	if(!epoch || strcmp(epoch_type, "GPS")) {
		XLAL_PRINT_ERROR("missing epoch or Type != \"GPS\"");
		ligolw_array_free(array);
		return NULL;
	}
	if(XLALStrToGPS(&gps, epoch, NULL)) {
		XLAL_PRINT_ERROR("incomprehensible epoch");
		ligolw_array_free(array);
		return NULL;
	}

	if(array->n_dims != 2) {
		XLAL_PRINT_ERROR("Array has wrong number of dimensions (%d)", array->n_dims);
		ligolw_array_free(array);
		return NULL;
	}

	if(!array->dims[0].scale) {
		XLAL_PRINT_ERROR("missing Scale (deltaF)");
		ligolw_array_free(array);
		return NULL;
	}
	deltaF = strtod(array->dims[0].scale, NULL);

	if(!XLALParseUnitString(&sampleUnits, array->dims[0].unit)) {
		XLAL_PRINT_ERROR("incomprehensible or missing Units");
		ligolw_array_free(array);
		return NULL;
	}

	/* remove optional :array suffix from name */
	name = strdup(array->name);
	*strchrnul(name, ':') = '\0';

	series = XLALCreateREAL8FrequencySeries(
		name,
		&gps,
		ligolw_param_parse(ligolw_param_get(elem, "f0"), &f0_type).as_double,
		deltaF,
		&sampleUnits,
		array->dims[0].n
	);
	free(name);

	if(!series) {
		ligolw_array_free(array);
		return NULL;
	}
	if(f0_type != ligolw_cell_type_real_8) {
		XLAL_PRINT_ERROR("incomprehensible f0");
		XLALDestroyREAL8FrequencySeries(series);
		ligolw_array_free(array);
		return NULL;
	}

	for(i = 0; i < array->dims[0].n; i++)
		series->data->data[i] = ((double *) array->data)[2 * i + 1];

	ligolw_array_free(array);

	return series;
}
