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
#include <ctype.h>

#include "basic_t.h"
#include "btree_t.h"
#include "btree_io.h"

#include "mem_op.h"				/* not all libs have memmv() function */

static int16 insert_empty_mem_index(db_t *db, d_tab_entry *entry, BOOLEAN mem_only); 
static int16 insert_mem_index(db_t *db, d_tab_entry *entry, BOOLEAN mem_only); 
static int16 insert_disc_index(db_t *db, d_tab_entry *entry);
static int16 db_rec_exist(db_t *db, d_tab_entry *entry, BOOLEAN *found, BOOLEAN delete);


int16 open_db(db_t *db)
{
  int16 i, ret;
  char file_name[FNAME+EXT_SIZE];

  sprintf(file_name, "%s%s", db->name, CFG_EXT);
  db->cfgfile = fopen(file_name, "r+b");
  if (db->cfgfile == NULL)
    return ERR_CFG_FILE_OPEN;
  
  sprintf(file_name, "%s%s", db->name, DB_EXT);
  db->dbfile = fopen(file_name, "r+b");
  if (db->dbfile == NULL)
    return ERR_DB_FILE_OPEN;
  rewind(db->dbfile);

  if ((ret = read_db_stat(db)) != 0)
    return ret;
  sprintf(file_name, "%s%s", db->name, M_EXT);
  db->mfile = fopen(file_name, "r+b");
  if (db->mfile == NULL)
    return ERR_M_FILE_OPEN;
  rewind(db->mfile);
  
  sprintf(file_name, "%s%s", db->name, D_EXT);
  db->dfile = fopen(file_name, "r+b");
  if (db->dfile == NULL)
    return ERR_D_FILE_OPEN;
  rewind(db->dfile);
  
  if ((ret = read_mem(db)) != 0)
    return ret;
  if ((db->dtab = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL)
    return ERR_NO_MEM;
  db->dtab->ownkey = -1L;
  db->dtab->range = 0;
  db->dindex = 0;
  db->tindex = -1; /* On -1, traversal will bootstrap via db->dindex instead */
  db->changed = FALSE;
  for(i=0; i < max_disc; i++)
    db->dtab->tab[i].key = NULL;
  switch (db->key_type)
    {
    case integer16:
    case integer32:
    case floating32:
    case floating64:
      if ((db->upper_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      if ((db->lower_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->match_string = NULL;
      break;
    case string:
      if ((db->match_string = (char *)malloc(2*db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->upper_bound = NULL;
      db->lower_bound = NULL;
      break;
    }
  return 0;
}


int16 close_db(db_t *db) 
{
  int16 i,
        ret;

  if ((ret = write_db_stat(db)) != 0)
    return ret;
  if (db->dbfile != NULL)
    fclose(db->dbfile);
  db->dbfile = NULL;
  if (db->cfgfile != NULL)
    fclose(db->cfgfile);
  db->cfgfile = NULL;
  rewind(db->mfile);
  if ((ret = write_mem(db)) != 0)
    return ret;
  if (db->mfile != NULL)
    fclose(db->mfile);
  db->mfile = NULL;
  if (db->changed) 
    {
      ret = write_dtab(db, db->dtab);
    }
  if (db->dfile != NULL)
    fclose(db->dfile);
  db->dfile = NULL;
  if (db->match_string != NULL)
    free(db->match_string);
  else {
    if (db->upper_bound != NULL)
      free(db->upper_bound);
    if (db->lower_bound != NULL)
      free(db->lower_bound);
  }
  if (db->dtab != NULL)
    {
      for (i=0; i < max_disc; i++)
	{
	  if (db->dtab->tab[i].key != NULL)
	    free(db->dtab->tab[i].key);
	}
      free(db->dtab);
    }
  return ret;
}


d_tab_entry *get_next_db_rec(db_t *db) 
{
  BOOLEAN empty=TRUE;
#if 1
  if( db->tindex < 0 ) db->tindex = db->dindex;
  db->dindex = db->tindex;
#endif
  if (db->dindex  >= db->dtab->range) 
    /* end of table or table is empty */    
    {	
      if (db->changed == TRUE)
	if (write_dtab(db, db->dtab) != 0)
	  return NULL;
      while(empty == TRUE) 
	{     		   
	  /* skip empty tables */
	  if (db->dtab->next_dtab < 0)	
	    /* last table already loaded */
		return NULL;
	  else 
	    {
	      db->dtab->ownkey = db->dtab->next_dtab;
	      if (read_dtab(db, db->dtab) != 0)
		return NULL;
	      db->dindex = db->tindex = 0;
	    }
	  if (db->dtab->range > 0)
	    empty = FALSE;
	}
    }
  db->tindex++; /* Prime next record */
  return (d_tab_entry *)&db->dtab->tab[db->dindex];
}


d_tab_entry *get_prev_db_rec(db_t *db)
{
  BOOLEAN empty=TRUE;

#if 1
  if (db->tindex < 0) db->tindex = db->dindex;
  db->dindex = db->tindex - 1;
#else
  db->dindex--;
#endif
  if (db->dindex  < 0) 
    /* end of table or table is empty */
    {
      if (db->changed == TRUE)
	if (write_dtab(db, db->dtab) != 0)
	  return NULL;
      while(empty) 
	{
	  if (db->dtab->prev_dtab < 0)	
	    /* first table already loaded */
	    return NULL;
	  else 
	    {
	      db->dtab->ownkey = db->dtab->prev_dtab;
	      if (read_dtab(db, db->dtab) != 0)
		return NULL;
	      db->dindex = db->tindex = db->dtab->range-1;
	    }
	  if (db->dtab->range > 0)
	    empty = FALSE;
	}
    }
  return (d_tab_entry *)&db->dtab->tab[db->dindex];
}


static int16 insert_index(db_t *db, d_tab_entry *entry) 
{
  int16        ret;
  m_tab_t     *tmp_mtab1=NULL, 
              *tmp_mtab2=NULL;
  char        *tmp_key;
  
  tmp_mtab1 = db->mtab;
  if ((tmp_key = (char *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  /* save current key for eventual restore */
  memcpy(tmp_key, entry->key, db->key_size);
  if (db->mtab->range == 0) {
      /* no entries in index --> database is empty */
      if ((ret =  insert_empty_mem_index(db, entry, FALSE)) != 0)
	return ret;
  } else {
    if ((ret = insert_mem_index(db, entry, FALSE)) != 0)
      return ret;
  }
  if (tmp_mtab1 != db->mtab) 
    { 		
      /* tree did grow so allocate new root node */
      if ((tmp_mtab2 = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL)
	return ERR_NO_MEM;
      tmp_mtab2->range=1;
      tmp_mtab2->next_on_disc=FALSE;
      tmp_mtab2->key[0] =  entry->key;
      entry->key = tmp_key;
      tmp_mtab2->u[1].mtab = db->mtab;
      tmp_mtab2->u[0].mtab = tmp_mtab1;
      db->mtab = tmp_mtab2;
    }
  else
    free(tmp_key);
  return 0;
}


static int16 insert_empty_mem_index(db_t *db, d_tab_entry *entry, BOOLEAN mem_only)
{
  int16    ret;

  db->curr_dkey = 0;
  if (!mem_only)
    if ((ret = insert_disc_index(db, entry)) != 0) 
      return ret;
  if ((db->mtab->key[0] = (void *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  memcpy(db->mtab->key[0], entry->key, db->key_size);
  db->mtab->u[0].dtab = 0;             /* offset of first block on disk */
  db->mtab->u[1].dtab = db->d_tab_size; /* offset of second block on disk */
  db->mtab->range++;
  return 0;
}


static int16 insert_mem_index(db_t *db, d_tab_entry *entry, BOOLEAN mem_only) 
{
  int16    left, 
           right, 
           pos, 
           length,  
           res,  
           ret;
  m_tab_t *tmp_mtab1=NULL, 
          *tmp_mtab2=NULL, 
          *tmp_mtab3=NULL;

  tmp_mtab3 = db->mtab;					/* save old pointer */

  /* binary search for normal insert */
  left = 0;
  right = db->mtab->range - 1;
  while (left <= right) 
    {
      pos = (left + right) /2;
      switch(db->key_type)
	{
	case integer16:
	  res = *(int16 *)entry->key >= *(int16 *)db->mtab->key[pos];
	  break;
	case integer32:
	  res = *(int32 *)entry->key >= *(int32 *)db->mtab->key[pos];
	  break;
	case floating32:
	  res = *(float32 *)entry->key >= *(float32 *)db->mtab->key[pos];
	  break;
	case floating64:
	  res = *(float64 *)entry->key >= *(float64 *)db->mtab->key[pos];
	  break;
	case string:
	  res = strncmp(entry->key, db->mtab->key[pos], db->key_size);
	  break;
   }
      if (res <= 0) 
	right = pos - 1;
      else 
	left = pos + 1;
 
    }
  if (res > 0) 								
    /* greater than last entry */
    ++pos;
  if (!db->mtab->next_on_disc) 
    {				
      /* continue search */
      tmp_mtab1 = db->mtab->u[pos].mtab;
      db->mtab = tmp_mtab1;
      if ((ret = insert_mem_index(db, entry, mem_only)) != 0) 
	{
	  return ret;
	}
      if (tmp_mtab1 == db->mtab) 
	{ 	
	  /* table not split */
	  db->mtab = tmp_mtab3;	                /* restore old pointer */
	  return 0;
	}
    }
  else 
    {					  	/* insert in d_tab  */
      if (mem_only) 
	return 0;
      db->curr_dkey = db->mtab->u[pos].dtab;
      if ((ret = insert_disc_index(db, entry)) != 0)
	return ret;
      if (db->curr_dkey == db->mtab->u[pos].dtab) 
	/* table not divided */
	return 0;
    }

  tmp_mtab1 = tmp_mtab3;
  if (tmp_mtab1->range >= max_mem) 
    {			
      /* table must be split */
      if ((tmp_mtab2 = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL) {
	return ERR_NO_MEM;
      }
      if (pos < max_mem - max_mem/2) 
	{					
	  /* insert in old table */
	  length = max_mem - max_mem/2;	    	/* # of elements to copy */
	}
      else 
	{
	  pos -= max_mem - max_mem/2;
	  tmp_mtab3 = tmp_mtab2;
	  length = max_mem/2;
	}
      memcpy(&(tmp_mtab2->key[0]), &(tmp_mtab1->key[max_mem-length]), length*sizeof(void *));
      memcpy(&(tmp_mtab2->u[0]), &(tmp_mtab1->u[max_mem-length]), (length+1)*sizeof(int32));
      tmp_mtab2->range = length;
      tmp_mtab1->range -= length;
    }

  length = tmp_mtab3->range - pos;
  if (length > 0) 
    {
      memmv(&(tmp_mtab3->key[pos+1]), &(tmp_mtab3->key[pos]), length*sizeof(void *));
      memmv(&(tmp_mtab3->u[pos+2]), &(tmp_mtab3->u[pos+1]), length*sizeof(int32));
    }

  if ((tmp_mtab3->key[pos] = (char *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  memcpy(tmp_mtab3->key[pos], entry->key, db->key_size);
  tmp_mtab3->range++;
  
  if (tmp_mtab1->next_on_disc) 
    {
      tmp_mtab3->u[pos+1].dtab = db->curr_dkey;
    }
  else 
    {
      tmp_mtab3->u[pos+1].mtab = db->mtab;
    }
  
  if (tmp_mtab2 != NULL) 
    {		
      /* new table created */
      tmp_mtab2->next_on_disc = tmp_mtab1->next_on_disc;
      --tmp_mtab1->range;
      entry->key = tmp_mtab1->key[tmp_mtab1->range];
      /* return key of new table */
      db->mtab = tmp_mtab2;				/* return address of new table */
    }
  else
    db->mtab = tmp_mtab3;
  return 0;
}


static int16 insert_disc_index(db_t *db, d_tab_entry *entry) 
{
  int16       length, 
              left,
              right,
              pos,
              res,
              ret,
              i;
  d_tab_t    *tmp_dtab1=NULL, 
             *tmp_dtab2=NULL;
  if (db->curr_dkey != db->dtab->ownkey) 
    {
      /* table not in memory */
      if (db->changed == TRUE)
	if ((ret = write_dtab(db, db->dtab)) != 0)
	  return ret;
      db->dtab->ownkey = db->curr_dkey;
      if ((ret = read_dtab(db, db->dtab)) != 0)
	return ret;
    }
  if (db->dtab->range == 0) 
    {
      /* table is empty, just insert and leave */
      db->dtab->range++;
      memcpy(&db->dtab->tab[0], entry, sizeof(d_tab_entry));
      if ((db->dtab->tab[0].key = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      memcpy(db->dtab->tab[0].key, entry->key, db->key_size); 
     db->changed = TRUE;
      return 0;
    }

  /* binary search for normal insert */
  left = 0;
  right = db->dtab->range - 1;
  while (left <= right) 
    {
      pos = (left + right) /2;
      switch(db->key_type)
	{
	case integer16:
	  res = *(int16 *)entry->key >= *(int16 *)db->dtab->tab[pos].key;
	  break;
	case integer32:
	  res = *(int32 *)entry->key >= *(int32 *)db->dtab->tab[pos].key;
	  break;
	case floating32:
	  res = *(float32 *)entry->key >= *(float32 *)db->dtab->tab[pos].key;
	  break;
	case floating64:
	  res = *(float64 *)entry->key >= *(float64 *)db->dtab->tab[pos].key;
	  break;
	case string:
	  res = strncmp(entry->key, db->dtab->tab[pos].key, db->key_size);
	  break;
   }
      if (res <= 0) 
	right = pos - 1;
      else 
	left = pos + 1;
 
    }
  if (res > 0) 								
    /* greater than last entry */
    ++pos;  
  if (db->dtab->range >= max_disc) 
    {					
      /* table is full */
      if ((tmp_dtab1 = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL) 
	  return ERR_NO_MEM;
      for (i=0; i < max_disc; i++)
	tmp_dtab1->tab[i].key = NULL;
      if (pos < max_disc - max_disc/2) 
	{		
	  /* insert in old table */
	  tmp_dtab2 = db->dtab;
	  /* pointer to table where new entry will be inserted */
	  length = max_disc - max_disc/2;
	  /* # of elements to be copied */
	}
      else 
	{
	  pos -= max_disc - max_disc/2; 
	  /* position in new table */
	  tmp_dtab2 = tmp_dtab1;
	  /* pointer to table where new entry will be inserted */
	  length = max_disc/2;
	  /* # of elements to be copied */
	}
      memcpy(&tmp_dtab1->tab[0], &db->dtab->tab[max_disc-length], length*sizeof(d_tab_entry));
      tmp_dtab1->range = length;
      tmp_dtab1->next_dtab = db->dtab->next_dtab;
      tmp_dtab1->prev_dtab = db->dtab->ownkey;
      db->dtab->range -= length;
      if (fseek(db->dfile, 0L, SEEK_END) != 0)
	return ERR_D_FILE_SEEK;
      /* append new table to file */
      if ((tmp_dtab1->ownkey = ftell(db->dfile)) == -1)
	return ERR_D_FILE_SEEK;
      /* append new table to file */
      db->dtab->next_dtab = tmp_dtab1->ownkey;
    }
  else
    {
      tmp_dtab2 = db->dtab;
    }
 
  /* insert in  table tmp_dtab2 */
  length = tmp_dtab2->range - pos;
  if (length > 0) 
      memmv(&tmp_dtab2->tab[pos+1], &tmp_dtab2->tab[pos], length*sizeof(d_tab_entry));
  tmp_dtab2->range++;
  memcpy(&tmp_dtab2->tab[pos], entry, sizeof(d_tab_entry));
  if ((tmp_dtab2->tab[pos].key = (void *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  memcpy(tmp_dtab2->tab[pos].key, entry->key, db->key_size);
  db->changed = TRUE;
  if (tmp_dtab1 != NULL) 
    {
      /* new table inserted */
      db->curr_dkey = tmp_dtab1->ownkey;       /* return dkey of new table */
      memcpy(entry->key, db->dtab->tab[db->dtab->range-1].key, db->key_size);
      if ((ret = write_dtab(db, tmp_dtab1)) != 0)
	return ret;
      if (tmp_dtab1->next_dtab >= 0) 
	{
	  tmp_dtab1->ownkey = tmp_dtab1->next_dtab;
	  if ((ret = read_dtab(db, tmp_dtab1)) != 0)
	    return ret;
	  tmp_dtab1->prev_dtab = db->curr_dkey;
	  if ((ret = write_dtab(db, tmp_dtab1)) != 0)
	    return ret;
	}
      for (i=0; i < tmp_dtab1->range; i++)
	free(tmp_dtab1->tab[i].key);
      free(tmp_dtab1);
      return 0;
    }
  return 0;
}


int16 delete_record(db_t *db, d_tab_entry *entry) 
{
  BOOLEAN	 		found=FALSE;
  int16			ret;
  
  if ((ret = db_rec_exist(db, entry, &found, FALSE)) != 0)
    return ret;
  if (!found || entry->del) 
    {	
      /* does not exist or already deleted */
      return 0;
    }
  /* delete */
  if ((ret = db_rec_exist(db, entry, &found, TRUE)) != 0)
    return ret;
  if (found && entry->del) {
    db->num_db_eld++;
    db->num_db_el--;
    return 0;
  }
  else
    return ERR_DELETE;
}


int16 undelete_record(db_t *db, d_tab_entry *entry) 
{
  BOOLEAN 		found=FALSE;
  int16				ret;
  
  if ((ret = db_rec_exist(db, entry, &found, FALSE)) != 0)
    return ret;
  
  if (!found || entry->del) 
    {
      /* does not exist or not deleted */
      return 0;
    }

  /* undelete */
  if ((ret = db_rec_exist(db, entry, &found, TRUE)) != 0)
    return ret;
  
  if (found && !entry->del) {
    db->num_db_eld--;
    db->num_db_el++;
    return 0;
  }
  return ERR_UNDELETE;
}


int16 new_record(db_t *db, void *rec[]) 
{
  int16				ret;
  d_tab_entry 	                entry;
  BOOLEAN			exists;
  

  entry.db_key = -1L;
  if ((entry.key = (void *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  memcpy(entry.key, rec[0], db->key_size);
  /* new records _must_ have db_key == -1L */
  if ((ret = db_rec_exist(db, &entry, &exists, FALSE)) != 0)
    return ret;
  if (exists) 
    {
      return ERR_REC_EXISTS;
    }
  if ((ret = db_compl_rec_write(db, &entry, rec)) != 0)
    return ret;
  if ((ret = insert_index(db, &entry)) != 0)
    return ret;
  free(entry.key);
  db->num_db_el++;
  return 0;
}


int16 save_record(db_t *db, d_tab_entry *entry, void *rec[]) 
{
  int16				ret;
  BOOLEAN			changed;
  
  /* check if key has changed */
  switch (db->key_type)
    {
    case integer16:
      if (*(int16 *)entry->key != *(int16 *)rec[0])
	changed = TRUE;
      break;
    case integer32:
      if (*(int32 *)entry->key != *(int32 *)rec[0])
	changed = TRUE;
      break;
    case floating32:
      if (*(float32 *)entry->key != *(float32 *)rec[0])
	changed = TRUE;
      break;
    case floating64:
      if (*(float64 *)entry->key != *(float64 *)rec[0])
	changed = TRUE;
      break;
    case string:
      if (strcmp((char *)entry->key, (char *)rec[0]) != 0)
	changed = TRUE;
      break;
    }
  if (changed == TRUE)
    {
      if ((ret = delete_record(db, entry)) != 0)
	return ret;
      memcpy(entry->key, rec[0], db->key_size);
      if ((ret = new_record(db, rec)) != 0)
	return ret;
    }
  else
      if ((ret = db_rec_write(db, entry->db_key, rec)) != 0)
      return ret;
  return 0;
}


int16 get_record(db_t *db, d_tab_entry *entry, void *rec[]) 
{
  int 			        ret;
  BOOLEAN			exists;
  
  if ((ret = db_rec_exist(db, entry, &exists, FALSE)) != 0)
    return ret;
  if (!exists)
    return ERR_REC_DOES_NOT_EXIST;
  ret = db_rec_read(db, entry->db_key, rec);
  return ret;
}



static int16 search_mem_index(db_t *db, d_tab_entry *entry) 
{
  m_tab_t	 *tmp1=NULL,
                 *tmp2=NULL;
  int16		  left,
                  right,
                  pos,
                  res;
  
  if (db->mtab->range == 0) 
    {
      /* no entries in index so db is empty */
      db->curr_dkey = 0L;
      /* insert in first dtab */
      return 0;
    }
  db->curr_dkey = -1L;
  tmp1 = db->mtab;
  
  for (;;) 
    {
      tmp2 = tmp1;
      /* Binary Search */
      left = 0;
      right = tmp2->range - 1;
      while (left <= right) 
	{
	  pos = (left + right) /2;
	  switch(db->key_type)
	    {
	    case integer16:
	      res = *(int16 *)entry->key >= *(int16 *)tmp2->key[pos];
	      break;
	    case integer32:
	      res = *(int32 *)entry->key >= *(int32 *)tmp2->key[pos];
	      break;
	    case floating32:
	      res = *(float32 *)entry->key >= *(float32 *)tmp2->key[pos];
	      break;
	    case floating64:
	      res = *(float64 *)entry->key >= *(float64 *)tmp2->key[pos];
	      break;
	    case string:
	      res = strncmp(entry->key, tmp2->key[pos], db->key_size);
	  break;
	}
	  if (res <= 0)
	    right = pos - 1;
	  else if (res > 0)
	    left = pos + 1;
	}

      if (res > 0)
	/* greater than last entry */
	++pos;
      
      if (tmp2->next_on_disc) 
	{
	  db->curr_dkey = tmp2->u[pos].dtab;
	  return 0;
	}
      tmp1 = tmp2->u[pos].mtab;
    }
}

d_tab_entry *copy_dtab_entry( d_tab_entry *e1, d_tab_entry *e2, db_t *db) {
   e1->del = e2->del;
   e1->db_key = e2->db_key;
   memcpy(e1->key, e2->key, db->key_size);
   return e1;
}

static int16 search_disc_index(db_t *db, d_tab_entry *entry, BOOLEAN *found, BOOLEAN delete) 
{
  int16			left,
  right,
  res;
  
  *found = FALSE;
  if (db->dtab->ownkey != db->curr_dkey)
    {
      /* table must be read in */
      if (db->changed)
	if ((res = write_dtab(db, db->dtab)) != 0)
	  return res;
      db->dtab->ownkey = db->curr_dkey;
      if ((res = read_dtab(db, db->dtab)) != 0)
	return res;
    }
  if (db->dtab->range <= 0) {
    return 0;
  }
  /* Binary Search */
  left = 0;
  right = db->dtab->range - 1;
  while ((left <= right) && !(*found)) 
    {
      db->dindex = (left + right) / 2;
	  switch(db->key_type)
	    {
	    case integer16:
	      if (*(int16 *)entry->key > *(int16 *)db->dtab->tab[db->dindex].key)
		res = 1;
	      else if (*(int16 *)entry->key == *(int16 *)db->dtab->tab[db->dindex].key)
		res = 0;
	      else 
		res = -1;
	      break;
	    case integer32:
	      if ( *(int32 *)entry->key > *(int32 *)db->dtab->tab[db->dindex].key)
		res = 1;
	      else if (*(int32 *)entry->key == *(int32 *)db->dtab->tab[db->dindex].key)
		res = 0;
	      else
		res = -1;
	      break;
	    case floating32:
	      if (*(float32 *)entry->key > *(float32 *)db->dtab->tab[db->dindex].key)
		res = 1;
	      else if (*(float32 *)entry->key == *(float32 *)db->dtab->tab[db->dindex].key)
		res = 0;
	      else
		res = -1;
	      break;
	    case floating64:
	      if (*(float64 *)entry->key > *(float64 *)db->dtab->tab[db->dindex].key)
		res = 1;
	      else if (*(float64 *)entry->key == *(float64 *)db->dtab->tab[db->dindex].key)
		res = 0;
	      else res = 1;
	      break;
	    case string:
 	  	if (db->case_sensitive) 
  		      res = strncmp(entry->key, db->dtab->tab[db->dindex].key, db->key_size);
 		else
 		   {
 		      int	a, b, i, l;
 		      char	*p = (char *)entry->key;
 		      char	*q = (char *)db->dtab->tab[db->dindex].key;
		      char	*r;
 		      res = 0; /* Assume they're equal. */
		      if ( !(r=strchr(p, WILD_CARD_SIGN))
		      &&   !(r=strchr(p, JOKER_SIGN))
		      &&  !(r=strchr(p, '\0'))) {
			  fprintf(stderr,"Non-null terminated search key.\n");
			  exit(-1);
		      }
#if 1
		      db->pos = r - p + 1;
		      for (i=0; i < db->pos; ++i)
#else
		      if ((l = strlen(p)) < strlen(q))
			l = strlen(q);
 		      for (i=0; i < l; ++i) 
#endif
 		         {
 			    a = toupper(p[i]);
 			    b = toupper(q[i]);
 			    if ( a != b )
 			       {
 				  res = a - b;
 				  break;
 			       }
 			 }
 		    }
  	  break;
	}
      if (res == 0) {
	*found = TRUE;
      } else if (res > 0) {
	left = db->dindex + 1;
      } else {
	right = db->dindex - 1;
    }
    }
  if (!*found) 
    {
      if (res > 0) {
	++db->dindex;
      }
      return 0;
    }
  
  if (*found) 
    {
      if (delete) 
	{
	  if (entry->del)
	    db->dtab->tab[db->dindex].del = FALSE;		/* undelete */
	  else
	    db->dtab->tab[db->dindex].del = TRUE;		/* delete */
	}
#if 0
      *entry = db->dtab->tab[db->dindex];
      /* Note - direct assignments don't work so well if the
       * structure in question has dynamic members...
       * at least not in C (maybe C++, but not C).
       */
#else
      copy_dtab_entry( entry, &(db->dtab->tab[db->dindex]), db );
#endif
      if (delete) 
	{
	  db->changed = TRUE;
	}
    }
  return 0;
}


static int16 db_rec_exist(db_t *db, d_tab_entry *entry, BOOLEAN *found, BOOLEAN delete) 
{
  int16		res;
  
  *found = FALSE;
  res = search_mem_index(db, entry);
  if ( res != 0) return res;
  if (db->curr_dkey < 0L) {
    return 0;
  }
  res = search_disc_index(db, entry, found, delete);
  return  res;
}


int remove_deleted_records(db_t *db) 
{
  int16 		i,
  j,
  ret;
  void 		        **rec;
  d_tab_t		*tmp_dtab;
  
  if ((tmp_dtab = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL)
    return ERR_NO_MEM;
  
  if ((rec = (void *)malloc(db->num_fields * sizeof(void *))) == NULL)
    return ERR_NO_MEM;
  for (i=0; i < db->num_fields; i++)
      {
	if ((rec[i] = (void *)malloc(db->fields[i].size)) == NULL)
	  return ERR_NO_MEM;
      }
  if (db->changed)
    if ((ret = write_dtab(db, db->dtab)) != 0)
      return ret;
  db->dtab->ownkey = 0L;
  if ((ret = read_dtab(db, db->dtab)) != 0)
    return ret;
  
  for(;;) 
    {
      j = 0;
      memcpy(tmp_dtab, db->dtab, sizeof(d_tab_t));
      for(i=0; i < tmp_dtab->range; ++i) 
	{
	  if (tmp_dtab->tab[i].del == FALSE) 
	    {
	      db->dtab->tab[j] = tmp_dtab->tab[i];
	      j++;
	    }
	  else 
	    {
	      if ((ret = db_compl_rec_read(db, &db->dtab->tab[i], rec)) != 0)
		return ret;
	      db->dtab->tab[i].del = TRUE;			
	      /* mark as deleted */
	      if ((ret = db_compl_rec_write(db, &db->dtab->tab[i], rec)) != 0)
		return ret;
	    }
	  
	}
      db->dtab->range = j;
      if ((ret = write_dtab(db, db->dtab)) != 0)
	return ret;
      if (db->dtab->next_dtab == -1L)
	break;
      db->dtab->ownkey = db->dtab->next_dtab;
      if ((ret = read_dtab(db, db->dtab)) != 0)
	return ret;
    }
  free((char *)tmp_dtab);
  for (i=0; i < db->num_fields; i++)
    free(rec[i]);
  free(rec);
  db->num_db_eld = 0;
  return 0;
}



static int16 build_index(db_t *db) 
{
  int16				ret;
  m_tab_t			*tmp_mtab1=NULL,
  *tmp_mtab2=NULL;
  d_tab_entry		entry;
  char 			file_name[FNAME+EXT_SIZE];
/*  char			*dummy=NULL; */
  
  if (db->dtab->range == 0
      && db->dtab->next_dtab == -1L
      && db->dtab->prev_dtab == -1L) 
    {
      return 0;
    }
  if (db->mtab != NULL)
    return ERR_BUILD_INDEX;
  
  sprintf(file_name, "%s%s",db->name, M_EXT);
  fclose(db->mfile);
  
  if ((db->mfile = fopen(file_name, "r+b")) == NULL)
    return ERR_M_FILE_OPEN;
  
  if ((db->mtab = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL)
    return ERR_NO_MEM;
  
  db->mtab->next_on_disc = TRUE;
  db->mtab->range = 0;
  
  if ((entry.key = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
  if (db->changed)
    if ((ret = write_dtab(db, db->dtab)) != 0)
      return ret;
  db->dtab->ownkey = 0L;
  if ((ret = read_dtab(db, db->dtab)) != 0)
    return ret;

  for(;;) 
    {
      if (db->dtab->next_dtab == -1L)	  /* last dtab has no entry in mtab */
	break;
      tmp_mtab1 = db->mtab;
      
      memcpy(entry.key, db->dtab->tab[db->dtab->range-1].key, db->key_size);
      db->curr_dkey = db->dtab->next_dtab;
      if (db->mtab->range == 0) {
	if ((ret = insert_empty_mem_index(db, &entry, TRUE)) != 0)
	  return ret;
      }
      else
	if ((ret = insert_mem_index(db, &entry, TRUE)) != 0)
	  return ret;
      if (tmp_mtab1 != db->mtab) {	/* b+tree has grown */
	if ((tmp_mtab2 = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL)
	  return ERR_NO_MEM;
	tmp_mtab2->range = 1;
	tmp_mtab2->next_on_disc = FALSE;
	tmp_mtab2->key[0] = entry.key;
	tmp_mtab2->u[0].mtab = db->mtab;
	tmp_mtab2->u[1].mtab = tmp_mtab1;
	db->mtab = tmp_mtab2;
	if ((entry.key = (void *)malloc(db->key_size)) == NULL)
	  return ret;
      }
      db->dtab->ownkey = db->dtab->next_dtab;
      if ((ret = read_dtab(db, db->dtab)) != 0)
	return ret;
    }
  return  0;
}


static int16 compress_dfile(db_t *db) 
{
  int16			i,
  j,
  length,
  ret;
  d_tab_t		*tmp_dtab;
  FILE		*tmp_dfile=NULL,
                *old_dfile;

  BOOLEAN		empty;
  char 		file_name[FNAME+EXT_SIZE];
  
  if ((tmp_dfile = fopen(TMP_FN, "w+b")) == NULL)
    return ERR_TMP_FILE_OPEN;
  old_dfile = db->dfile;
  if ((tmp_dtab = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL)
    return ERR_NO_MEM;
  if (db->changed)
    if ((ret = write_dtab(db, db->dtab)) != 0)
      return ret;
  db->dtab->ownkey = 0L;
  if ((ret = read_dtab(db, db->dtab)) != 0)
    return ret;
  j = 0;
  i = 0;
  tmp_dtab->ownkey = -1L;
  tmp_dtab->next_dtab = 0L;
  
  for(;;) 
    {
      if (db->dtab->range-j <= max_disc-i)
	length = db->dtab->range-j;
      else
	length = max_disc-i;
      if (length > 0) 
	{
	  memcpy(&tmp_dtab->tab[i], &db->dtab->tab[j], length*sizeof(d_tab_entry));	/* copy tab */
	  i += length;
	  j += length;
	}
      if (j >= db->dtab->range) 
	{	
	  /* read new table */
	  empty = TRUE;
	  j = 0;
	  while(empty) 
	    {
	      if (db->dtab->next_dtab < 0)				/* last table read */
		break;
	      else {
		db->dfile = old_dfile;
		db->dtab->ownkey = db->dtab->next_dtab;
		if ((ret = read_dtab(db, db->dtab)) != 0)
		  return ret;
	      }
	      if (db->dtab->range > 0)
		empty = FALSE;
	    }
	}
      
      if (i == max_disc || empty) 
	{
	  tmp_dtab->range = i;
	  tmp_dtab->prev_dtab = tmp_dtab->ownkey;
	  tmp_dtab->ownkey = tmp_dtab->next_dtab;
	  tmp_dtab->next_dtab = empty ? -1L : tmp_dtab->ownkey + db->d_tab_size;
	  db->dfile = tmp_dfile;
	  if ((ret = write_dtab(db, tmp_dtab)) != 0)
	    return ret;
	  i = 0;
	  if (empty)
	    break;
	}
    }
  if (tmp_dtab->ownkey == 0L) 
    {				
      /* compression has a db with less than max_disc entries as result */
      tmp_dtab->next_dtab = db->d_tab_size;
      if ((ret = write_dtab(db, tmp_dtab)) != 0)
	return ret;
      tmp_dtab->range = 0;
      tmp_dtab->prev_dtab = tmp_dtab->ownkey;
      tmp_dtab->ownkey = tmp_dtab->next_dtab;
      tmp_dtab->next_dtab = -1L;
      if ((ret = write_dtab(db, tmp_dtab)) != 0)
	return ret;
    }
  
  sprintf(file_name, "%s%s", db->name, D_EXT);
  fclose(old_dfile);
  fclose(tmp_dfile);
  /* delete old disk-index file */
  if (remove(file_name) != 0)
    return ERR_D_FILE_REMOVE;
  /* rename new (temporary) disk-index file */
  if (rename(TMP_FN, file_name) != 0)
    return ERR_TMP_FILE_RENAME;
  if ((db->dfile = fopen(file_name, "r+b")) == NULL)
    return ERR_D_FILE_OPEN;
  
  db->dtab->ownkey = 0L;
  if ((ret = read_dtab(db, db->dtab)) != 0)  /* reload table */
    return ret;
  free((char *)tmp_dtab);
  return 0;
}



int rebuild_indices(db_t *db) 
{
  int16				ret;
  
  rewind(db->mfile);
  if ((ret =  write_mem(db)) != 0)
    return ret;
  if (db->mtab != NULL)
    return ERR_REBUILD;
  if ((ret = compress_dfile(db)) != 0)
    return ret;
  if ((ret = build_index(db)) != 0)
    return ret;
  return 0;
}


int restore_db(db_t *db) 
{
  int16				j,
			       	ret;
  int32				i,
  num;
  BOOLEAN			exists;
  d_tab_entry		entry;
  void			**rec;
  char			file_name[FNAME+EXT_SIZE];

  sprintf(file_name, "%s%s", db->name, CFG_EXT);
  db->cfgfile = fopen(file_name, "r+b");
  if (db->cfgfile == NULL)
	return ERR_CFG_FILE_OPEN;

  sprintf(file_name, "%s%s", db->name, DB_EXT);
  db->dbfile = fopen(file_name, "r+b");
  if (db->dbfile == NULL)
	return ERR_DB_FILE_OPEN;

  if ((ret = read_db_stat(db)) != 0)
	return ret;

  if ((rec = (void **)malloc(db->num_fields * sizeof(void *))) == NULL)
	return ERR_NO_MEM;
  for (j=0; j < db->num_fields; j++)
	if ((rec[j] = (void *)malloc(db->fields[j].size)) == NULL)
		return ERR_NO_MEM;
  sprintf(file_name, "%s%s", db->name, M_EXT);
  db->mfile = fopen(file_name, "w+b");
  if (db->mfile == NULL)
	return ERR_M_FILE_OPEN;
  if ((db->mtab = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL)
	return ERR_NO_MEM;
  db->mtab->next_on_disc = TRUE;
  db->mtab->range = 0;

  sprintf(file_name, "%s%s", db->name, D_EXT);
  db->dfile = fopen(file_name, "w+b");
  if (db->dfile == NULL)
	return ERR_D_FILE_OPEN;
  if ((db->dtab = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL)
	return ERR_NO_MEM;
  db->dtab->ownkey = 0L;
  db->dtab->next_dtab = db->d_tab_size;
  db->dtab->prev_dtab = -1L;
  db->dtab->range = 0;
  if ((ret = write_dtab(db, db->dtab)) != 0)
	return ret;
  db->dtab->ownkey = db->d_tab_size;
  db->dtab->next_dtab = -1L;
  db->dtab->prev_dtab = 0L;
  db->dtab->range = 0;
  db->dindex = 0;
  db->changed = FALSE;
  if ((ret = write_dtab(db, db->dtab)) != 0)
	return ret;

  db->num_db_el = 0;
  db->num_db_eld = 0;
  db->first_del_rec = -1L;

  if (fseek(db->dbfile, 0L, SEEK_END) != 0)
	return ERR_DB_FILE_SEEK;
  num = ftell(db->dbfile) / (long)db->glob_rec_size;
  rewind(db->dbfile);

  for(i=0L; i < num; i++)
	{
	  entry.db_key = ftell(db->dbfile);
	  db->curr_dkey = entry.db_key;
      if ((ret = db_compl_rec_read(db, &entry, rec)) != 0)
		return ret;
	  if (entry.del)
	{
	  /* link permanently deleted records */
	  entry.db_key = db->first_del_rec;
	  db->first_del_rec = db->curr_dkey;
	  continue;
	}
      if ((ret = db_rec_exist(db, &entry, &exists, FALSE)) != 0)
	return ret;
      if (exists)
	/* hmmmmmm, something's really gone crazy, throw the db away */
	return ERR_FATAL;
      if ((ret = insert_index(db, &entry)) != 0)
	return ret;
      db->num_db_el++;
	}
  for(j=0; j < db->num_fields; j++)
	free(rec[j]);
  free(rec);
  switch (db->key_type)
    {
    case integer16:
    case integer32:
    case floating32:
    case floating64:
      if ((db->upper_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      if ((db->lower_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->match_string = NULL;
      break;
    case string:
      if ((db->match_string = (char *)malloc(2*db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->upper_bound = NULL;
      db->lower_bound = NULL;
      break;
    }
  return 0;
}


int16 new_db(db_t *db, char *ddl_name)  
{
  /* initialize new (empty) database */
  int16 	ret,
                i;
  char 		file_name[FNAME+EXT_SIZE],
                line[80],
                *tmp;
  FILE          *ddl_file;


  ddl_file = fopen(ddl_name, "r");
  if (ddl_file == NULL)
    return ERR_DDL_FILE_OPEN;
  rewind(ddl_file);

  db->fields = NULL;
  db->record_size = 0;
  db->glob_rec_size = 0;
  /* parse the ddl-file */
  for(i=0;;)
    {
      if (fgets(line, 80, ddl_file) == NULL)
	{
	  if (feof(ddl_file))
	    break;
	  else
	    return ERR_DDL_FILE_READ;
	}
      /* skip comment lines */
      if (line[0] == '%' || line[0] == '#' || strlen(line) == 0)
	continue;
      if (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == ';')
	line[strlen(line)-1] = '\0';
       if (i == 0)
	{
	  if ((db->fields = (field_desc_t *)malloc(sizeof(field_desc_t))) == NULL) 
	    return ERR_NO_MEM;
	}
      else 
	{
	  if ((db->fields = (field_desc_t *)realloc(db->fields, (i+1) * sizeof(field_desc_t))) == NULL)
	    return ERR_NO_MEM;
	}
	  tmp = strtok(line, ";");
       if ((db->fields[i].name = (char *)malloc(strlen(tmp)+1)) == NULL)
	return ERR_NO_MEM;
      strcpy(db->fields[i].name, tmp);
      tmp = strtok(NULL, ";");
      switch (tmp[0])
	{ 
	case 'i':
	  if (strstr(tmp, "16") != NULL) 
	    {
	      db->fields[i].type = integer16;
	      db->fields[i].size = 2;
	  }
	  else 
	    {
	      db->fields[i].type = integer32;
	      db->fields[i].size = 4;
	  }
	  break;
	case 'f':
	  if (strstr(tmp, "32") != NULL)
	    {
	      db->fields[i].type = floating32;
	      db->fields[i].size = 4;
	    }
	  else
	    {
	      db->fields[i].type = floating64;
	      db->fields[i].size = 8;
	    }
	  break;
	case 's':
	  db->fields[i].type = string;
	  tmp = strtok(NULL, ";");
	  db->fields[i].size = atoi(tmp);
	  break;
	}
      db->record_size += db->fields[i].size;
       i++;
    }
  fclose(ddl_file);
  db->num_fields = i;
  db->key_size = db->fields[0].size;
  db->key_type = db->fields[0].type;
  db->m_tab_size = sizeof(m_tab_t) + max_mem * (db->key_size - sizeof(void *));
  db->d_tab_size = sizeof(int16) + 3 * sizeof(int32) + max_disc * (db->key_size + sizeof(BOOLEAN) + sizeof(int32));
  db->glob_rec_size = db->record_size + 1; /* delete flag */
  
  sprintf(file_name, "%s%s", db->name, CFG_EXT);
  db->cfgfile = fopen(file_name, "w+b");
  if (db->cfgfile == NULL)
    return ERR_CFG_FILE_OPEN;
  
  sprintf(file_name, "%s%s", db->name, DB_EXT);
  db->dbfile = fopen(file_name, "w+b");
  if (db->dbfile == NULL)
    return ERR_DB_FILE_OPEN;
  rewind(db->dbfile);
  
  sprintf(file_name, "%s%s", db->name, M_EXT);
  db->mfile = fopen(file_name, "w+b");
  if (db->mfile == NULL)
    return ERR_M_FILE_OPEN;
  rewind(db->mfile);
  if ((db->mtab = (m_tab_t *)malloc(sizeof(m_tab_t))) == NULL)
    return ERR_NO_MEM;
  db->mtab->next_on_disc = TRUE;
  db->mtab->range = 0;
  
  sprintf(file_name, "%s%s", db->name, D_EXT);
  db->dfile = fopen(file_name, "w+b");
  if (db->dfile == NULL)
    return ERR_D_FILE_OPEN;
  rewind(db->dfile);
  if ((db->dtab = (d_tab_t *)malloc(sizeof(d_tab_t))) == NULL)
    return ERR_NO_MEM;
  db->dtab->ownkey = 0L;
  db->dtab->next_dtab = db->d_tab_size;
  db->dtab->prev_dtab = -1L;
  db->dtab->range = 0;
  if ((ret = write_dtab(db, db->dtab)) != 0)
    return ret;
  db->dtab->ownkey = db->d_tab_size;
  db->dtab->next_dtab = -1L;
  db->dtab->prev_dtab = 0L;
  db->dtab->range = 0;
  db->dindex = 0;
  db->changed = FALSE;
  if ((ret = write_dtab(db, db->dtab)) != 0)
    return ret;

  db->first_del_rec = -1;		 /* first permanently deleted record */
  db->num_db_el = 0;		         /* # of active records in db */
  db->num_db_eld = 0;		         /* # of deleted records in db */
  if ((ret = write_db_stat(db)) != 0)
    return ret;
  switch (db->key_type)
    {
    case integer16:
    case integer32:
    case floating32:
    case floating64:
      if ((db->upper_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      if ((db->lower_bound = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->match_string = NULL;
      break;
    case string:
      if ((db->match_string = (char *)malloc(2*db->key_size)) == NULL)
	return ERR_NO_MEM;
      db->upper_bound = NULL;
      db->lower_bound = NULL;
      break;
    }

  return 0;
}


int16 remove_from_index(db_t *db) 
{
  int16	length;
  
  length = db->dtab->range - db->dindex - 1;
  free(db->dtab->tab[db->dindex].key);
  if (length > 0)
    memmv(&db->dtab->tab[db->dindex], &db->dtab->tab[db->dindex+1], length);
  db->dtab->range--;
  db->changed = TRUE;
  return 0;
}


static BOOLEAN match(db_t *db, char *str) 
{

  /* match db->match_string against str */
  /* if your system has own function use it or program your own */
  return TRUE;
}



static BOOLEAN match_str(db_t *db, char *true_string, BOOLEAN *stop) 
{

  /* This function uses the special structure of a b+tree to speedup searches */
  /* for records with a match_string containing no wildcard/joker at the      */
  /* first position. It compares the beginning of the match_string till the   */
  /* first wildcard/joker against the beginning of the true_string. If this   */
  /* fails (== not equal) the program can stop and must not have a look at    */
  /* the next (previous) record after a get_next_rec() (get_prev_rec())       */
  /* command since it cannot match either.                                    */
  
  BOOLEAN 	ok;
  
  ok = FALSE;
  switch (db->match_type) 
    {
    case 1:
      /* joker or wildcard at first position */
      ok = match(db, true_string);
      break;
    case 2:
      /* no joker or wildcard at first position */
      ok = match(db, true_string);
      if (!ok) 
	{
	  if (db->case_sensitive) 
	    {
	      if (strncmp(true_string, db->match_string, db->pos) != 0)
		*stop = TRUE;
	    }
	  else 
	    {
	      /* not case_sensitive */
	      short	i;
	      for (i=0; i < db->pos; ++i) 
		{
		  if (toupper(true_string[i]) != toupper(db->match_string[i])) 
		    {
		      *stop = TRUE;
		      break;
		    }
		}
	    }
	}
      break;
    }
  return ok;
}



static int16 set_db_pos(db_t *db) 
{
  int16 ret;
  
  db->tindex = -1;  /* On -1, traversal will bootstrap via db->dindex instead */
  if (db->match_type == 2) 
    {
      d_tab_entry tmp_entry;
      BOOLEAN found;

      if ((tmp_entry.key = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
	switch(db->key_type)
	{
	case integer16:
	case integer32:
	case floating32:
	case floating64:
	  memcpy(tmp_entry.key, db->lower_bound, db->key_size);
	  break;
	case string:
	  /* no joker/wildcard at beginning of match_string */
	  memcpy(tmp_entry.key, db->match_string, db->key_size);
	  ((char *)tmp_entry.key)[db->pos] = '\0';
	}
      /* set db-pointer to first record that matches till the first wildcard/joker */
      if ((ret = search_mem_index(db, &tmp_entry)) != 0)
	return ret;
      if (db->curr_dkey < 0L)
	return 0;
      return search_disc_index(db, &tmp_entry, &found, FALSE);
    }
  else 
    {
      /* joker/wildcard at beginning of match_string */
      /* position db at first record */
      if (db->changed == TRUE)
	if ((ret = write_dtab(db, db->dtab)) != 0)
	  return ret;
      db->dtab->ownkey = 0L;
      db->dindex = 0;
      return read_dtab(db, db->dtab);
    }
}


int16 set_select_criteria(db_t *db) 
{
  int16 			ret;
  char			tmp[3];

  db->pos = -1;
  
  switch(db->key_type)
    {
    case integer16:
      if (*(int16 *)db->upper_bound < *(int16 *)db->lower_bound) 
	/* everyhting selected */
	db->match_type = 0;
      else
	/* exact match or range match */
	db->match_type = 2;
      break;
    case integer32:
      if (*(int32 *)db->upper_bound < *(int32 *)db->lower_bound)
	/* everyting selected */
	db->match_type = 0;
      else
	db->match_type = 2;
      break;
    case floating32:
      if (*(float32 *)db->upper_bound < *(float32 *)db->lower_bound)
	db->match_type = 0;
      else
	db->match_type = 2;
      break;
    case floating64:
      if (*(float64 *)db->upper_bound < *(float64 *)db->lower_bound)
	db->match_type = 0;
      else
	db->match_type = 2;
      break;
    case string:
      sprintf(tmp, "%c%c", JOKER_SIGN, WILD_CARD_SIGN);
      if (strlen(db->match_string) == 0) 
	{
	  db->pos = 0;
	  db->match_type = 0;				/* empty string --> everything selected */
	}
      else 
	{
	  db->pos = strcspn(db->match_string, tmp);		/* position of first wildcard or joker */
	  /* analyze match_string */
	  if (db->pos == 0) 
	    {
	      if (db->match_string[0] == JOKER_SIGN && db->match_string[1] == '\0')
		db->match_type = 0;		/* everything selected by single joker */
	      else
		db->match_type = 1;		/* joker or wildcard at beginning */
	    }
	  else
	    db->match_type = 2;			/* it's possible to reduce search */
	}
    }
  if ((ret = set_db_pos(db)) != 0)
    return ret;
  return 0;
}


int16 get_next_match(db_t *db, d_tab_entry *entry) 
{
  BOOLEAN			ok,
  stop;
  d_tab_entry		*tmp_entry;
  
  ok = TRUE;
  stop = FALSE;
  tmp_entry = NULL;
  while ((tmp_entry = get_next_db_rec(db)) != NULL) 
    {
      switch(db->key_type)
	{
	case integer16:
	case integer32:
	case floating32:
	case floating64:
	  break;
	case string:
	  if (db->match_type != 0)
	    ok = match_str(db, tmp_entry->key, &stop);
	  if (stop)
	    /* last possible record read and doesn't match */
	    return -1;
	  if (!ok)
	    continue;
	}
      entry->db_key = tmp_entry->db_key;
      entry->del = tmp_entry->del;
      memcpy(entry->key, tmp_entry->key, db->key_size);
      return 0;
    }
  return -1;						/* end of database reached */
}


int16 get_prev_match(db_t *db, d_tab_entry *entry) 
{
  BOOLEAN			ok,
  stop;
  d_tab_entry		*tmp_entry;
  
  ok = TRUE;
  stop = FALSE;
  tmp_entry = NULL;
  while ((tmp_entry = get_prev_db_rec(db)) != NULL) 
    {
      if (db->match_type != 0)
	ok = match_str(db, tmp_entry->key, &stop);
      if (stop)
	return -1;				/* first possible record read and doesn't match */
      if (!ok)
	continue;
      memcpy(entry, tmp_entry, sizeof(d_tab_entry));
      return 0;
    }
  return -1;						/* beginning of database reached */
}

