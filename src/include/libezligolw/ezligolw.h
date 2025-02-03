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


#include <complex.h>
#include <stdio.h>
#include <stdint.h>
#include <libezligolw/ezxml.h>


enum ligolw_cell_type {
	ligolw_cell_type_char_s,
	ligolw_cell_type_char_v,
	ligolw_cell_type_ilwdchar,
	ligolw_cell_type_ilwdchar_u,
	ligolw_cell_type_blob,
	ligolw_cell_type_lstring,
	ligolw_cell_type_int_2s,
	ligolw_cell_type_int_2u,
	ligolw_cell_type_int_4s,
	ligolw_cell_type_int_4u,
	ligolw_cell_type_int_8s,
	ligolw_cell_type_int_8u,
	ligolw_cell_type_real_4,
	ligolw_cell_type_real_8,
	ligolw_cell_type_complex_8,
	ligolw_cell_type_complex_16
};

union ligolw_cell {
	int64_t as_int;
	uint64_t as_uint;
	double as_double;
	double complex as_double_complex;
	const char *as_string;
	const unsigned char *as_blob;
};


struct ligolw_array {
	const char *name;
	enum ligolw_cell_type type;
	char delimiter;
	int n_dims;
	struct ligolw_array_dim {
		int n;	/* must be first so a Dim can be type cast to int */
		const char *name;
		const char *unit;
		const char *start;
		const char *scale;
	} *dims;
	void *data;
};


struct ligolw_table {
	const char *name;
	char delimiter;
	int n_columns;
	struct ligolw_table_column {
		const char *name;
		struct ligolw_table *table;
		enum ligolw_cell_type type;
	} *columns;
	int n_rows;
	struct ligolw_table_row {
		struct ligolw_table *table;
		union ligolw_cell *cells;
	} *rows;
};


const char *ligolw_strip_name(const char *, const char *);
enum ligolw_cell_type ligolw_type_name_to_enum(const char *);
const char *ligolw_type_enum_to_name(enum ligolw_cell_type);
size_t ligolw_type_enum_to_size(enum ligolw_cell_type);
union ligolw_cell *ligolw_cell_from_txt(union ligolw_cell *, enum ligolw_cell_type, char *);
char *ligolw_cell_to_txt(char *, union ligolw_cell, enum ligolw_cell_type);
int ligolw_cell_to_c(const union ligolw_cell *, enum ligolw_cell_type, void *);

ezxml_t ligolw_array_get(ezxml_t, const char *);
struct ligolw_array *ligolw_array_parse(ezxml_t);
void ligolw_array_free(struct ligolw_array *);

int ligolw_table_default_row_callback(struct ligolw_table *, struct ligolw_table_row, void *);
ezxml_t ligolw_table_get(ezxml_t, const char *);
struct ligolw_table *ligolw_table_parse(ezxml_t, int (*)(struct ligolw_table *, struct ligolw_table_row, void *), void *);
void ligolw_table_free(struct ligolw_table *);
int ligolw_table_get_column(struct ligolw_table *, const char *, enum ligolw_cell_type *);
int ligolw_table_print(FILE *, struct ligolw_table *);
union ligolw_cell ligolw_row_get_cell(const struct ligolw_table_row *, const char *, enum ligolw_cell_type *);


struct ligolw_unpacking_spec {
	void *dest;
	const char *name;
	enum ligolw_cell_type type;
	enum ligolw_column_flags {
		LIGOLW_COLUMN_FLAGS_NONE	= 0x00,
		LIGOLW_COLUMN_FLAGS_REQUIRED	= 0x01,
	} flags;
};

int ligolw_unpacking_row_builder(struct ligolw_table *, struct ligolw_table_row, void *);

const char *ligolw_time_parse(ezxml_t, const char **);
ezxml_t ligolw_time_get(ezxml_t, const char *);

union ligolw_cell ligolw_param_parse(ezxml_t, enum ligolw_cell_type *);
ezxml_t ligolw_param_get(ezxml_t, const char *);
