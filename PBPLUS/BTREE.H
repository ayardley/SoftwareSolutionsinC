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

#ifndef PB_BTREE_T
#include "btree_t.h"
#endif
extern int new_db(db_t *db, char *ddl_name);
extern int open_db(db_t *db);
extern int close_db(db_t *db);
extern int delete_record(db_t *db, d_tab_entry *entry);
extern int undelete_record(db_t *db, d_tab_entry *entry);
extern int save_record(db_t *db, d_tab_entry *entry, void *rec[]);
extern int new_record(db_t *db, void *rec);
extern int get_record(db_t *db, d_tab_entry *entry, void *rec[]);
extern int remove_deleted_records(db_t *db);
extern int rebuild_indices(db_t *db);
extern int restore_db(db_t *db);
extern int remove_from_index(db_t *db);
extern int set_select_criteria(db_t *db);
extern int get_next_match(db_t *db, d_tab_entry *entry);
extern int get_prev_match(db_t *db, d_tab_entry *entry);

