/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by John Greve.  Not derived from licensed software.
 * From the book "Software Solutions in C", edited by Dale Schumacher.
 *
 * Permission is granted to anyone to use this software for any
 * purpose on any computer system, and to redistribute it in any way,
 * subject to the following restrictions:
 *
 *   1. The author is not responsible for the consequences of use of
 *	this software, no matter how awful, even if they arise
 *	from defects in it.
 *
 *   2. The origin of this software must not be misrepresented, either
 *	by explicit claim or by omission.
 *
 *   3. Altered versions must be plainly marked as such, and must not
 *	be misrepresented (by explicit claim or omission) as being
 *	the original software.
 *
 *   4. This notice must not be removed or altered.
 */

/*
 *	addr.c -- example name and address database application
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	/* for isatty() */
#include "btree.h"

typedef struct addr {
	char	last[40];
	char	first[20];
	char	address[40];
	int32	zip;
} ADDR;

#define	ADDR_NUM_FIELDS		(4)

ADDR *
load_addr(db_t *db, char *last)
{
	d_tab_entry	entry;
	ADDR *		addr;
	char		key[sizeof(addr->last)];
	void *		pp[ADDR_NUM_FIELDS];
	int		rv;

	entry.key = (void *)key;
	addr = (ADDR *)malloc(sizeof(ADDR));
	if (addr == NULL) {
		fprintf(stderr,
			"load_addr: out of memory\n");
		return (ADDR *)0;
	}
	pp[0] = (void *)addr->last;
	pp[1] = (void *)addr->first;
	pp[2] = (void *)addr->address;
	pp[3] = (void *)&addr->zip;
	strncpy(db->match_string, last, sizeof(addr->last));
	db->match_string[sizeof(addr->last) - 1] = '\0';
	db->case_sensitive = TRUE;
	rv = set_select_criteria(db);
	if (rv != 0) {
		fprintf(stderr,
			"load_addr: error %d setting select criteria\n", rv);
		goto fail;
	}
	rv  = get_next_match(db, &entry);
	if (rv != 0) {
		fprintf(stderr,
			"load_addr: error %d locating record\n", rv);
		goto fail;
	}
	rv  = get_record(db, &entry, pp);
	if (rv != 0) {
		fprintf(stderr,
			"load_addr: error %d loading record\n", rv);
		goto fail;
	}
	return addr;
fail:
	free(addr);
	return (ADDR *)0;
}

BOOLEAN
save_addr(db_t *db, ADDR *addr)
{
	int		rv;
	void *		pp[ADDR_NUM_FIELDS];

	pp[0] = (void *)addr->last;
	pp[1] = (void *)addr->first;
	pp[2] = (void *)addr->address;
	pp[3] = (void *)&addr->zip;
	rv  = new_record(db, pp);
	if ((rv != 0) && (rv != ERR_REC_EXISTS)) {
		fprintf(stderr,
			"save_addr: error %d updating record.\n", rv);
		return FALSE;
	}
	return TRUE;
}

void
dump_addr(FILE *f, ADDR *addr)
{
	fprintf(f, "%.*s;%.*s;%.*s;%05ld\n",
		sizeof(addr->last) - 1,
		addr->last,
		sizeof(addr->first) - 1,
		addr->first,
		sizeof(addr->address) - 1,
		addr->address,
		(long)addr->zip);
}

BOOLEAN
read_field(char *data, int size, int delim, FILE *f)
{
	int c;

	memset(data, '\0', size);
	for (;;) {
		c = getc(f);
		if (c == EOF) {
			return FALSE;
		}
		if (c == delim) {
			return TRUE;
		}
		if (size > 0) {
			*data++ = c;
			--size;
		}
	}
}

BOOLEAN
add_recs(db_t *db, FILE *f)
{
	ADDR		addr;
	char		nbuf[6];

	while (read_field(addr.last, sizeof(addr.last), ';', f)) {
		if (!read_field(addr.first, sizeof(addr.first), ';', f)
		||  !read_field(addr.address, sizeof(addr.address), ';', f)
		||  !read_field(nbuf, sizeof(nbuf), '\n', f)) {
			return FALSE;
		}
		addr.zip = atol(nbuf);
		if (!save_addr(db, &addr)) {
			return FALSE;
		}
	}
	if (!feof(f)) {
		return FALSE;
	}
	return TRUE;
}

BOOLEAN
dump_recs(db_t *db, FILE *f)
{
	d_tab_entry	entry;
	void **		pp;
	void *		p;
	int		i;
	int		rv;

	entry.key = (void *)malloc(db->key_size);
	if (entry.key == NULL) {
		fprintf(stderr, "dump_recs: out of memory\n");
		return FALSE;
	}
	pp = (void **)calloc(db->num_fields, sizeof(void *));
	if (pp == NULL) {
		fprintf(stderr, "dump_recs: out of memory\n");
		return FALSE;
	}
	for (i = 0; i < db->num_fields; ++i) {
		p = (void *)malloc(db->fields[i].size);
		if (p == NULL) {
			fprintf(stderr, "dump_recs: out of memory\n");
			return FALSE;
		}
		pp[i] = p;
	}
	switch (db->key_type) {
	case integer16:
		*(int16 *)db->lower_bound = 1;
		*(int16 *)db->upper_bound = -1;
		break;
	case integer32:
		*(int32 *)db->lower_bound = 1;
		*(int32 *)db->upper_bound = -1;
		break;
	case floating32:
		*(float32 *)db->lower_bound = 1;
		*(float32 *)db->upper_bound = -1;
		break;
	case floating64:
		*(float64 *)db->lower_bound = 1;
		*(float64 *)db->upper_bound = -1;
		break;
	case string:
		strcpy(db->match_string, "*");
		db->case_sensitive = TRUE;
		break;
	}
	rv = set_select_criteria(db);
	if (rv != 0) {
		fprintf(stderr,
			"dump_recs: error %d setting select criteria\n", rv);
		return FALSE;
	}
	while ((rv = get_next_match(db, &entry)) == 0) {
		rv  = get_record(db, &entry, pp);
		if (rv != 0) {
			fprintf(stderr,
				"dump_recs: error %d loading record\n", rv);
			return FALSE;
		}
		/* generic record display */
		for (i = 0; i < db->num_fields; ++i) {
			switch (db->fields[i].type) {
			case integer16:
				fprintf(f, "%ld", (long)*((int16 *)pp[i]));
				break;
			case integer32:
				fprintf(f, "%ld", (long)*((int32 *)pp[i]));
				break;
			case floating32:
				fprintf(f, "%g", (double)*((float32 *)pp[i]));
				break;
			case floating64:
				fprintf(f, "%g", (double)*((float64 *)pp[i]));
				break;
			case string:
				fprintf(f, "%.*s",
					db->fields[i].size - 1,
					(char *)pp[i]);
				break;
			}
			fputc(((i < db->num_fields - 1) ? ';' : '\n'), f);
		}
	}
	if (rv != -1) {
		fprintf(stderr,
			"dump_recs: error %d locating record\n", rv);
		return FALSE;
	}
	for (i = 0; i < db->num_fields; ++i) {
		free(pp[i]);
	}
	free(pp);
	free(entry.key);
	return TRUE;
}

void
usage(void)
{
	fprintf(stderr, "usage:\n\
	addr <datafile		create new database from ascii data\n\
	addr >datafile		dump database to ascii datafile\n\
	addr lastname		retrieve record for ``lastname''\n\
	addr			interactively add entries from keyboard\n\
		ascii field format: ``lastname;firstname;address;zip''\n");
}

int
main(int argc, char **argv)
{
	db_t addr_db;
	ADDR *addr;
	int rv;

	strncpy(addr_db.name, "addr", sizeof(addr_db.name));
	addr_db.name[sizeof(addr_db.name) - 1] = '\0';
	switch (argc) {
	case 1:			/* add records from stdin or dump to stdout */
		if (isatty(fileno(stdin))) {
			rv = open_db(&addr_db);
			if (rv != 0) {
				fprintf(stderr,
					"addr: error %d opening database\n", rv);
				exit(EXIT_FAILURE);
			}
			if (isatty(fileno(stdout))) {
				/* interactively add records to db */
				rv = add_recs(&addr_db, stdin);
			} else {
				/* dump all records in db */
				rv = dump_recs(&addr_db, stdout);
			}
		} else {
			/* create new db */
			rv = new_db(&addr_db, "addr.ddl");
			if (rv != 0) {
				fprintf(stderr,
					"addr: error %d creating database\n", rv);
				exit(EXIT_FAILURE);
			}
			rv = add_recs(&addr_db, stdin);
		}
		if (rv == FALSE) {
			exit(EXIT_FAILURE);
		}
		break;
	case 2:			/* retrieve record matching argument */
		rv = open_db(&addr_db);
		if (rv != 0) {
			fprintf(stderr,
				"addr: error %d opening database\n", rv);
			exit(EXIT_FAILURE);
		}
		addr = load_addr(&addr_db, argv[1]);
		if (addr) {
			dump_addr(stdout, addr);
			free(addr);
		}
		break;
	default:
		usage();
		exit(EXIT_FAILURE);
	}
	rv = close_db(&addr_db);
	if (rv != 0) {
		fprintf(stderr, "addr: error %d closing database\n", rv);
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
