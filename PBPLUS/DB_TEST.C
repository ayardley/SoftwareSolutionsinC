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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "basic_t.h"
#include "btree_t.h"
#include "btree.h"

#define NUM_RECS 1000


db_t            db;
int             i,
		ret;
d_tab_entry     entry;
void          **rec;

#pragma argsused
int main(int argc, char *argv[]) 
{
  
  strcpy(db.name, "test");
  if ((ret = new_db(&db, "db.ddl")) != 0) 
    {
      printf("error %d in new_db()", ret);
      exit(0);
    }
  if ((rec = (void *)malloc(db.num_fields * sizeof(void *))) == NULL)
    return ERR_NO_MEM;
  for (i=0; i < db.num_fields; i++)
    if ((rec[i] = (void *)malloc(db.fields[i].size)) == NULL)
      return ERR_NO_MEM;
  if ((entry.key = (void *)malloc(db.key_size)) == NULL)
    return ERR_NO_MEM;
  switch(db.key_type)
	{
	case integer16:
	case integer32:
	case floating32:
	case floating64:
	  if ((db.upper_bound = (void *)malloc(db.key_size)) == NULL)
	return ERR_NO_MEM;
	  if ((db.lower_bound = (void *)malloc(db.key_size)) == NULL)
	return ERR_NO_MEM;
	  db.match_string = NULL;
	  break;
	case string:
      if ((db.match_string = (char *)malloc(db.key_size)) == NULL)
	return ERR_NO_MEM;
	    db.upper_bound = NULL;
      db.lower_bound = NULL;
      break;
    }
  switch(db.key_type)
    {
    case integer16:
      break;
    case integer32:
    case floating32:
    case floating64:
      srand48(1234L);
      break;
    case string:
      break;
    }
  printf("start loop\n");
  for (i=0; i< NUM_RECS; i++) 
    {
      printf("#%d : ", i);
      switch(db.key_type)
	{
	case integer16:
	  *(int16 *)rec[0] = rand();
	  printf("%8d", *(int16 *)rec[0]);
	  break;
	case integer32:
	  *(int32 *)rec[0] = mrand48();
	  printf("%8ld", *(int32 *)rec[0]);
	  break;
	case floating32:
	  *(float32 *)rec[0] =  (float32)mrand48()/(float32)mrand48();
	  printf("%3.8f", *(float32 *)rec[0]);
	  break;
	case floating64:
	  *(float64 *)rec[0] = (float64)drand48();
	  printf("%3.8f", *(float64 *)rec[0]);
	  break;
	case string:
	  sprintf((char *)rec[0], "%05d", rand());
	  printf("%s", (char *)rec[0]);
	  break;
	}

      if ((ret = new_record(&db, rec)) != 0) 
	{
	  if (ret == ERR_REC_EXISTS) 
	    {
	      printf(" part exists\n");
	    }
	  else 
	    {
	      printf(" error %d in new_record()", ret);
	      exit(0);
	    }
	  
	}
      printf("\n");
    }
  
  /*      if you want to rebuild the db after a crash open it using
	  following code. it rebuilds all indexes out of the datafile
	  
	  if ((ret = rebuild_indices(&db)) != 0) 
	  {
	  printf("error %d in rebuild_indices()", ret);
	  exit(0);
	  }
	  */
  printf("Do selection: \n");
  switch(db.key_type)
    {
    case integer16:
      *(int16 *)db.lower_bound = 0;
      *(int16 *)db.upper_bound = -1;
      break;
    case integer32:
      *(int32 *)db.lower_bound = 0;
      *(int32 *)db.upper_bound = -1;
      break;
    case floating32:
      *(float32 *)db.lower_bound = 0.0;
      *(float32 *)db.upper_bound = -1.0;
      break;
    case floating64:
      *(float64 *)db.lower_bound = 0.0;
      *(float64 *)db.upper_bound = -1.0;
      break;
    case string:
      sprintf(db.match_string, "*");
      db.case_sensitive = FALSE;
    }
  if ((ret = set_select_criteria(&db)) != 0) 
    {
      printf("error %d in set_select_criteria()", ret);
      exit(0);
    }
  printf("dtab = %ld, dindex = %d\n", db.dtab->ownkey, db.dindex);
  i = 0;
  while ((ret = get_next_match(&db, &entry)) >= 0) 
    {
      if (ret > 0) 
	{
	  printf("Error %d in get_next_match()", ret);
	  exit(0);
	}
      printf("%05d: %5ld  ", i++, entry.db_key);
      switch(db.key_type)
	{
	case integer16:
	  printf("%8d\n", *(int16 *)entry.key);
	  break;
	case integer32:
	  printf("%12ld\n", *(int32 *)entry.key);
	  break;
	case floating32:
	  printf("%8.5f\n", *(float32 *)entry.key);
	  break;
	case floating64:
	  printf("%8.5f\n", *(float64 *)entry.key);
	  break;
	case string:
	  printf("%s\n", (char *)entry.key);
	}
    }
  printf("\n");
  if ((ret = close_db(&db)) != 0) 
    {
      printf("error %d in close_db()", ret);
      exit(0);
    }
  return 0;
}
