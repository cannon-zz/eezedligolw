/*
 * Copyright (C) 2026  Kipp Cannon
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
#include <ezligolw/ezligolw.h>


int ligolw_lal_table_unpack_row(struct ligolw_table *, struct ligolw_table_row, struct ligolw_unpacking_spec *);


int ligolw_sim_burst_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);
int ligolw_sim_inspiral_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);
int ligolw_time_slide_row_callback(struct ligolw_table *, struct ligolw_table_row *, void *);


REAL8FrequencySeries *ligolw_REAL8FrequencySeries_parse(ezxml_t);


LALDict *ligolw_PSDs(ezxml_t);
LALDict *ligolw_PSDsFromFile(const char *);
