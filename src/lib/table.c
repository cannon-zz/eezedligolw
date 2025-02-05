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
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
#include <libezligolw/ezligolw.h>


/*
 * Extract the meaningful portion of a table name.  Returns a pointer to
 * the last colon-delimited substring before an optional ":table" suffix.
 */


static const char *ligolw_strip_table_name(const char *Name)
{
	return ligolw_strip_name(Name, "table");
}


/*
 * Extract the meaningful portion of a column name.  Returns a pointer to
 * the last colon-delimited substring.
 */


static const char *ligolw_strip_column_name(const char *Name)
{
	return ligolw_strip_name(Name, NULL);
}


/*
 * Default row builder call-back.  Appends the contents of the row object
 * to the rows array in table.
 */


int ligolw_table_default_row_callback(struct ligolw_table *table, struct ligolw_table_row *row, void *ignored)
{
	table->rows = realloc(table->rows, (table->n_rows + 1) * sizeof(*table->rows));
	table->rows[table->n_rows] = *row;
	table->n_rows++;
	free(row);
	return 0;
}


/*
 * retrieve the ligolw_cell corresponding to the named column from a row.
 * if type is not NULL, the cell type is stored in the location it points
 * to.
 */


union ligolw_cell ligolw_row_get_cell(const struct ligolw_table_row *row, const char *name, enum ligolw_cell_type *type)
{
	return row->cells[ligolw_table_get_column(row->table, name, type)];
}


/*
 * Parse an ezxml_t Table element into a struct ligolw_table structure.  If
 * row_callback() is NULL, then the default row builder is used, which
 * inserts the rows directly into the ligolw_table structure.  Calling code
 * can provide it's own function, which will be called after each row is
 * constructed.  This allows the data contained in each row to be
 * redirected, for example to store it in an application-specific type, or
 * process it on-the-fly to produce some kind of output directly.  The
 * call-back function will be passed the address of the current
 * ligolw_table structure as its first argument, the address of a newly
 * allocated row structure as its second, and the callback_data pointer as
 * its third argument.  The row_callback() function takes ownership of the
 * row structure, and is responsible for freeing all memory associated with
 * the object when it no longer requires it. The call-back returns 0 to
 * indicate success, non-zero to indicate failure.
 *
 * ligolw_table_parse() return the pointer to the new struct ligolw_table
 * structure on success, NULL on failure.
 */


struct ligolw_table *ligolw_table_parse(ezxml_t elem, int (row_callback)(struct ligolw_table *, struct ligolw_table_row *, void *), void *callback_data)
{
	struct ligolw_table *table;
	char *txt;
	ezxml_t column;
	ezxml_t stream;

	/* this simplifies error checking in calling code */
	if(!elem)
		return NULL;

	table = malloc(sizeof(*table));
	if(!table)
		return NULL;

	table->name = ligolw_strip_table_name(ezxml_attr(elem, "Name"));

	table->delimiter = '\0';
	table->n_columns = 0;
	table->columns = NULL;
	table->n_rows = 0;
	table->rows = NULL;

	for(column = ezxml_child(elem, "Column"); column; column = column->next) {
		table->columns = realloc(table->columns, (table->n_columns + 1) * sizeof(*table->columns));

		table->columns[table->n_columns].name = ligolw_strip_column_name(ezxml_attr(column, "Name"));
		table->columns[table->n_columns].table = table;
		table->columns[table->n_columns].type = ligolw_type_name_to_enum(ezxml_attr(column, "Type"));

		table->n_columns++;
	}

	stream = ezxml_child(elem, "Stream");
	if(!stream) {
		/* DTD allows Table to have 0 Stream children */
		return table;
	}

	table->delimiter = ezxml_attr(stream, "Delimiter") ? *ezxml_attr(stream, "Delimiter") : ',';

	if(!row_callback)
		row_callback = ligolw_table_default_row_callback;

	for(txt = stream->txt; txt && *txt; ) {
		int c;
		struct ligolw_table_row *row = malloc(sizeof(*row));
		union ligolw_cell *cells = malloc(table->n_columns * sizeof(*cells));
		if(!row || !cells) {
			free(row);
			free(cells);
			ligolw_table_free(table);
			return NULL;
		}
		row->table = table;
		row->cells = cells;

		for(c = 0; c < table->n_columns; c++) {
			char *end, *next;

			ligolw_next_token(&txt, &end, &next, table->delimiter);

			ligolw_cell_from_txt(&cells[c], table->columns[c].type, txt);

			/* null-terminate current token.  this does not
			 * interfer with the exit test for the loop over
			 * txt because end and next can only point to the
			 * same address if that address is the end of the
			 * text */
			*end = '\0';

			/* advance to next token */
			txt = next;
		}

		/* row_callback takes ownership of row */
		if(row_callback(table, row, callback_data)) {
			ligolw_table_free(table);
			return NULL;
		}
	}

	return table;
}


/*
 * Free a struct ligolw_table.
 */


void ligolw_table_free(struct ligolw_table *table)
{

	if(table) {
		int i;
		for(i = 0; i < table->n_rows; i++)
			free(table->rows[i].cells);
		free(table->rows);
		free(table->columns);
	}
	free(table);
}


/*
 * Get a column index by name from within a table.  Returns the index of
 * the column within table's columns array (and thus of the corresponding
 * cell within each row's cell array) or -1 on failure.  If type is not
 * NULL, the place it points to is set to the columns's cell_type.
 */


int ligolw_table_get_column(struct ligolw_table *table, const char *name, enum ligolw_cell_type *type)
{
	int i;

	for(i = 0; i < table->n_columns; i++)
		if(!strcmp(table->columns[i].name, name)) {
			/* found it */
			if(type)
				*type = table->columns[i].type;
			return i;
		}

	/* couldn't find that column name */
	if(type)
		*type = -1;
	return -1;
}


/*
 * Find an ezxml_t Table element in a document.  If name is NULL the first
 * element found is reported, otherwise the elment's Name attribute must
 * match name (ignoring an optional :... suffix).
 */


ezxml_t ligolw_table_get(ezxml_t elem, const char *name)
{
	return ligolw_elem_iter(elem, "Table", name);
}


/*
 * Utility to assist with unpacking a table row into alternate storage.
 * NOTE:  this is not a row builder call-back for use with
 * ligolw_table_parse(), it is meant to assist with writing call-backs.
 * The signature is incompatible, and this does not take ownership of the
 * row object, it will not row the row object's contents when it is
 * finished.
 */


int ligolw_table_unpack_row(struct ligolw_table *table, struct ligolw_table_row row, void *data)
{
	struct ligolw_unpacking_spec *spec;

	for(spec = data; spec->name; spec++) {
		enum ligolw_cell_type type;
		int c = ligolw_table_get_column(table, spec->name, &type);
		if(c < 0) {
			/* no column by that name */
			if(!(spec->flags & LIGOLW_COLUMN_FLAGS_REQUIRED))
				/* not required */
				continue;
			/* missing column is required */
			return spec - (struct ligolw_unpacking_spec *) data + 1;
		}
		if(spec->type != type) {
			/* type mismatch */
			return -(spec - (struct ligolw_unpacking_spec *) data + 1);
		}
		if(!spec->dest)
			/* column has a valid name and the correct type,
			 * but is ignored */
			continue;

		if(ligolw_cell_to_c(&row.cells[c], spec->type, spec->dest)) {
			/* spec provided an invalid type */
			return -(spec - (struct ligolw_unpacking_spec *) data + 1);
		}
	}

	return 0;
}


/*
 * Print a struct ligolw_table structure
 */


int ligolw_table_print(FILE *f, struct ligolw_table *table)
{
	char short_name[strlen(table->name) + 1];
	int r, c;

	/* create a version of the table name with the optional :table
	 * suffix removed */
	strcpy(short_name, table->name);
	{
	char *x = strchr(short_name, ':');
	if(x)
		*x = '\0';
	}

	/* print the table metadata */
	fprintf(f, "<Table Name=\"%s\">\n", table->name);
	for(c = 0; c < table->n_columns; c++)
		fprintf(f, "\t<Column Name=\"%s:%s\" Type=\"%s\"/>\n", short_name, table->columns[c].name, ligolw_type_enum_to_name(table->columns[c].type));
	fprintf(f, "\t<Stream Name=\"%s\" Type=\"Local\" Delimiter=\"%c\">\n", table->name, table->delimiter);

	/* print the rows */
	for(r = 0; r < table->n_rows; r++) {
		if(r)
			fprintf(f, "%c\n\t\t", table->delimiter);
		else
			fprintf(f, "\t\t");
		for(c = 0; c < table->n_columns; c++) {
			char *buf = ligolw_cell_to_txt(NULL, table->rows[r].cells[c], table->columns[c].type);
			if(c)
				fprintf(f, "%c", table->delimiter);
			fputs(buf, f);
			free(buf);
		}
	}

	/* finish 'er off */
	fprintf(f, "\n\t</Stream>\n</Table>\n");

	return 0;
}
