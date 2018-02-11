/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Michael Brandmaier.  Not derived from licensed software.
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

extern int16 write_dtab(db_t *db, d_tab_t *dtab);
extern int16 read_dtab(db_t *db, d_tab_t *dtab);

extern int16 read_mem(db_t *db);
extern int16 write_mem(db_t *db);

extern int16 db_rec_read(db_t *db, int32 db_key, void *rec[]);
extern int16 db_rec_write(db_t *db, int32 db_key, void *rec[]);

extern int16 db_compl_rec_read(db_t *db, d_tab_entry *entry, void *rec[]);
extern int16 db_compl_rec_write(db_t *db, d_tab_entry *entry, void *rec[]);

extern int16 read_db_stat(db_t *db);
extern int16 write_db_stat(db_t *db);
