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

/*
 * ex01.c - example one: name database.
 */

#include <stdio.h>
#include <ctype.h>
#include "btree.h"

#define MAX_LIMIT 60

typedef struct ex01_rec {
	char name[21];
	char date[11];
	int16 event;
	float64 nice;
} EX01_REC;

void **
bind_ex01_fields(db_t *db, EX01_REC *ex01_rec) {
	void **rec_binding;
	rec_binding = (void **)malloc( db->num_fields * sizeof(void *)  );
	rec_binding[0] = (void *) &(ex01_rec->name);
	rec_binding[1] = (void *) &(ex01_rec->date);
	rec_binding[2] = (void *) &(ex01_rec->event);
	rec_binding[3] = (void *) &(ex01_rec->nice);
	return rec_binding;
}


d_tab_entry *
copy_entry( d_tab_entry *dst, d_tab_entry *src, db_t *db ) {
	*dst = *src;
	dst->key = (void *)malloc(db->key_size);
	memcpy( dst->key, src->key, db->key_size );
	return( dst );
}

write_ddl(char *name) {
   FILE *f = fopen(name, "w");
#if 0
   fprintf(f, "TAG;string;20.\n" );
   fprintf(f, "DATE;string;10.\n" ); /* mm-dd-yyyy */
   fprintf(f, "EVENT#;int16;20.\n" );
   fprintf(f, "**NASTY*?!@# FIELD NAME[]{}`~:.,<>;float64.\n" );
#else
   fprintf(f, "TAG;string;20\n" );
   fprintf(f, "DATE;string;10\n" ); /* mm-dd-yyyy */
   fprintf(f, "EVENT#;int16;20\n" );
   fprintf(f, "NICE;float64\n" );
#endif
   fclose(f);
   return 0;
 }


db_t *
create_db(char *ddl_name) {
   db_t *my_db = (db_t *)malloc(sizeof(db_t));
   char *db_name = "ex01_db";
   int ret;

   printf("Ok - lets do smth. with the DDL '%s'\n", ddl_name );
   strncpy( my_db->name, db_name, sizeof(my_db->name) );
   if( (ret = new_db(my_db, ddl_name)) != 0 ) {
      printf("Error %d creating database from ddl file '%s'\n", ret, ddl_name);
      free(my_db);
      return (db_t *)0;
   }
   return my_db;
}

void
mockup_ex01(EX01_REC *r) {
   static int a = 1;
   static b = 0;
   int i;
   int base_char;
   ++b;
   a = -a;
   base_char = (b/5)%2 ? 'N' : 'n';
   for( i = 0; i < 5; ++i ) {
      r->name[i] = base_char + a*(b%13);
   }
   sprintf(r->name+i, "%05d", b);
   sprintf(r->date,"%02d/%02d/%04d",
	(b*17343)%12 + 1,
	(b*43437)%30 +1,
	(b*80123)%100 +1901);
   r->event = b;
   r->nice = (2.0 + 1.0 / 3.0)*b*b*b;
   return;
}



int
grow_db(db_t *db, int new_recs) {
   void **rec_binding;
   EX01_REC *r = (EX01_REC *)malloc(sizeof(EX01_REC));
   rec_binding = bind_ex01_fields(db, r);
   while( new_recs-- > 0 ) {
      mockup_ex01(r);
      printf("Writing '%s'\n", r->name);
      new_record(db, rec_binding);
   }
   free(r);
   free(rec_binding);
   return 0;
}
int 
test_db(db_t *db) {
   char b[1024];
   int i;
   int limit = 20;
   int entry_cnt, cur_entry;
   d_tab_entry entry_list[ MAX_LIMIT + 1 ];
   EX01_REC *r = (EX01_REC *)malloc(sizeof(EX01_REC));
   d_tab_entry entry;
   d_tab_entry *entry_ptr;
   void **rec_binding;

   db->case_sensitive = FALSE;
   entry.key = (void *)malloc(db->key_size);
   rec_binding = bind_ex01_fields(db, r);
   while( printf("PB+>"),fflush(stdout),gets(b) ) {
      switch(toupper(b[0])) {
      case 'C': /* Toggle case */
	 db->case_sensitive =  db->case_sensitive ? FALSE : TRUE;
	 printf("case %s\n", db->case_sensitive ? "aware" : "ignore" );
	 break;
      case 'F':
	 printf("Find key (case %s): '%s'\n",
		(db->case_sensitive ? "aware" : "ignore"),
		b+1);
	 strcpy(db->match_string,b+1);
	 if( (i=set_select_criteria(db)) != 0 ) {
	    printf("Error %d setting selection for find '%s'",i,b+1);
	    continue;
	 }
	 entry_cnt = 0;
	 for(limit = MAX_LIMIT; limit >= 0; --limit) {
	    i = get_next_match(db,&entry);
	    if( i > 0 ) {
		printf("Error locating next record %d.\n", i);
		break;
	    } else if( i < 0 ) {
		printf("No more records.\n");
		break;
	    }
	    printf( "%02d: key = '%s' @%d\n",
	       ++entry_cnt,
	       (char *)(entry.key),entry.db_key );
	 }
	 break;
      case 'D':
         printf("Dumping all records\n");
	 strcpy(db->match_string,"*");
	 if( (i=set_select_criteria(db)) != 0 ) {
	    printf("Error %d setting selection for 'dump-all'\n",i);
	    continue;
	 }
	 entry_cnt = 0;
	 for(limit = MAX_LIMIT; limit >= 0; --limit) {
	    i = get_next_match(db,&entry);
	    if( i > 0 ) {
		printf("Error locating next record %d.\n", i);
		break;
	    } else if( i < 0 ) {
		printf("No more records.\n");
		break;
	    }
	    printf("key = '%s' @%d\n", (char *)(entry.key),entry.db_key );
	    copy_entry( &(entry_list[entry_cnt++]), &entry, db );
	 }
	 printf("Dumping cached 'd_tab_entry' structs: entry_cnt=%d\n",entry_cnt);
	 for(cur_entry = 0; cur_entry < entry_cnt; ++cur_entry ) {
	    if( (i=get_record(db,&(entry_list[cur_entry]),rec_binding)) != 0 ) {
		printf("Error %d retrieving current record\n", i);
		break;
	    }
	    printf("By entry: rec->name = '%s'\n", r->name);
	 }
	 if(limit<=0) {
		printf("Check your code: the limit was reached.\n");
	 }
	 break;
      case 'Q':
	 goto bail_out;
         break;
      default: printf("Unrecognized command: '%c'\n",toupper(b[0]));
      }
   }
bail_out:
	return 0;
}

int main() {
	db_t *db01;
	char *ddl_name = "ex01.ddl";
	printf("%s: testing examples from M. Brandmier's article\n", __FILE__ );
	write_ddl( ddl_name );
	db01 = create_db( ddl_name );
	grow_db(db01, 50);
	test_db(db01);
}
