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
#include <stdlib.h>
#include <string.h>
		
#include "basic_t.h"
#include "btree_t.h"
#include "port_io.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#define line_len 20 /*  maximal length of field_name */

static int16 read_mtabs(db_t *db);
static int16 write_mtabs(db_t *db);


int write_dtab(db_t *db, d_tab_t *dtab) 
{
  void   *tmp;
  int     i;
  int16	 tmp16;
  int32	 tmp32;

  if ((tmp = malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  if (fseek(db->dfile, dtab->ownkey, SEEK_SET) != 0)
    return ERR_D_FILE_SEEK;
  tmp16 = conv_int16(dtab->range);
  if (fwrite(&tmp16, sizeof(int16), 1, db->dfile) <= 0)
    return ERR_D_FILE_WRITE;
  tmp32 = conv_int32(dtab->ownkey);
  if (fwrite(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_WRITE;
  tmp32 = conv_int32(dtab->next_dtab);
  if (fwrite(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_WRITE;
  tmp32 = conv_int32(dtab->prev_dtab);
  if (fwrite(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_WRITE;
  for(i=0; i < dtab->range; i++) 
    {
      switch(db->key_type) 
	{
	case integer16:
	  *(int16 *)tmp = conv_int16(*(int16 *)dtab->tab[i].key);
	  break;
	case integer32:
	  *(int32 *)tmp = conv_int32(*(int32 *)dtab->tab[i].key);
	  break;
	case floating32:
	  *(float32 *)tmp = conv_float32(*(float32 *)dtab->tab[i].key);
	  break;
	case floating64:
	  *(float64 *)tmp = conv_float64(*(float64 *)dtab->tab[i].key);
	  break;
	case string:
	  memcpy(tmp, dtab->tab[i].key, db->key_size);
	  break;
	}	
      if (fwrite(tmp, db->key_size, 1, db->dfile) <= 0)
	return ERR_D_FILE_WRITE;
      tmp16 = conv_int16(dtab->tab[i].del);
      if (fwrite(&tmp16, sizeof(int16), 1, db->dfile) <= 0)
	return ERR_D_FILE_WRITE;
      tmp32 = conv_int32(dtab->tab[i].db_key);
      if (fwrite(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
	return ERR_D_FILE_WRITE;
    }
  db->changed = FALSE;
  free(tmp);
  return 0;
}



int16 read_dtab(db_t *db, d_tab_t *dtab) 
{
  void   *tmp;
  int16     i,
            old_range,
	    tmp16;
  int32	    tmp32;

  old_range = dtab->range;
  if ((tmp = malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  if (fseek(db->dfile, dtab->ownkey, SEEK_SET) != 0)
    return ERR_D_FILE_SEEK;
  if (fread(&tmp16, sizeof(int16), 1, db->dfile) <= 0)
    return ERR_D_FILE_READ;
  dtab->range = conv_int16(tmp16);
  if (fread(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_READ;
  dtab->ownkey = conv_int32(tmp32);
  if (fread(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_READ;
  dtab->next_dtab = conv_int32(tmp32);
  if (fread(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
    return ERR_D_FILE_READ;
  dtab->prev_dtab = conv_int32(tmp32);
  if (dtab->range > old_range)
    {
      for (i=old_range; i < dtab->range; i++)
	if ((dtab->tab[i].key = (void *)malloc(db->key_size)) == NULL)
	  return ERR_NO_MEM;
    }
  else if (dtab->range < old_range)
    {
      for (i=dtab->range; i < old_range; i++)
	free(dtab->tab[i].key);
    }
  for (i=0; i < dtab->range; i++)
    {
      if (fread(tmp, db->key_size, 1, db->dfile) <= 0)
        return ERR_D_FILE_READ;
      switch (db->key_type) {
      case integer16:
	*(int16 *)dtab->tab[i].key = conv_int16(*(int16 *)tmp);
	break;
      case integer32:
	*(int32 *)dtab->tab[i].key = conv_int32(*(int32 *)tmp);
	break;
     case floating32:
	*(float32 *)dtab->tab[i].key = conv_float32(*(float32 *)tmp);
	break;
      case floating64:
	*(float64 *)dtab->tab[i].key = conv_float64(*(float64 *)tmp);
	break;
      case string:
	memcpy(dtab->tab[i].key, tmp, db->key_size);
	break;
      }
      if (fread(&tmp16, sizeof(int16), 1, db->dfile) <= 0)
        return ERR_D_FILE_READ;
      dtab->tab[i].del = conv_int16(tmp16);
      if (fread(&tmp32, sizeof(int32), 1, db->dfile) <= 0)
        return ERR_D_FILE_READ;
      dtab->tab[i].db_key = conv_int32(tmp32);
    }
  free(tmp);
  return 0;
}



int16 read_mem(db_t *db)
{
  rewind(db->mfile);
  return read_mtabs(db);
}


static int16 read_mtabs(db_t *db)
{
  int16 i, ret, tmp16;
  int32 tmp32;
  m_tab_t *tmp_mtab=NULL;
  char *tmp;

  if ((tmp_mtab = (m_tab_t *) malloc(sizeof(m_tab_t))) == NULL)
    return ERR_NO_MEM;
  if (fread(&tmp16, sizeof(int16), 1, db->mfile) < 1)
    return ERR_M_FILE_READ;
  tmp_mtab->next_on_disc = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->mfile) < 1)
    return ERR_M_FILE_READ;
  tmp_mtab->range = conv_int16(tmp16);
  if ((tmp = (char *)malloc(db->key_size)) == NULL)
    return ERR_NO_MEM;
  for (i=0; i < tmp_mtab->range; i++) 
    {
      if (fread(tmp, db->key_size, 1, db->mfile) < 1)
	return ERR_M_FILE_READ;
      if ((tmp_mtab->key[i] = (void *)malloc(db->key_size)) == NULL)
	return ERR_NO_MEM;
      switch(db->key_type) 
	{
	case integer16:
	  *(int16 *)(tmp_mtab->key[i]) = conv_int16(*(int16 *)tmp);
	  break;
	case integer32:
	  *(int32 *)(tmp_mtab->key[i]) = conv_int32(*(int32 *)tmp);
	  break;
	case floating32:
	  *(float32 *)(tmp_mtab->key[i]) = conv_float32(*(float32 *)tmp);
	  break;
	case floating64:
	  *(float64 *)(tmp_mtab->key[i]) = conv_float64(*(float64 *)tmp);
	  break;
	case string:
	  memcpy(tmp_mtab->key[i], tmp, db->key_size);
	}
    }
  free(tmp);
  if (tmp_mtab->next_on_disc)
    for(i=0; i <= tmp_mtab->range; i++) 
      {
	if (fread(&tmp32, sizeof(int32), 1, db->mfile) < 1)
	  return ERR_M_FILE_READ;
	tmp_mtab->u[i].dtab = conv_int32(tmp32);
      }
  else
    {
      for (i=0; i <= tmp_mtab->range; ++i)
	{
	  if ((ret = read_mtabs(db)) != 0)
	    return ret;
	  tmp_mtab->u[i].mtab = db->mtab;
	}
    }
  db->mtab = tmp_mtab;
  return 0;
}


int16 write_mem(db_t *db)
{
  rewind(db->mfile);
  return write_mtabs(db);
}


int16 write_mtabs(db_t *db)
{
  int16 i, tmp16, ret;
  int32 tmp32;
  m_tab_t *tmp_mtab;
  void *tmp;

  if (db->mtab == NULL)
    return 0;
  tmp_mtab = db->mtab;
  tmp16 = conv_int16(tmp_mtab->next_on_disc);
  if (fwrite(&tmp16, sizeof(int16), 1, db->mfile) < 1)
    return ERR_M_FILE_WRITE;
  tmp16 = conv_int16(tmp_mtab->range);
  if (fwrite(&tmp16, sizeof(int16), 1, db->mfile) < 1)
    return ERR_M_FILE_WRITE;
  if (db->key_type != string)
    if ((tmp = (char *)malloc(db->key_size)) == NULL)
      return ERR_NO_MEM;
  for(i=0; i < tmp_mtab->range; i++)
    { 
      switch(db->key_type) 
	{
	case integer16:
	  *(int16 *)tmp = conv_int16(*(int16 *)tmp_mtab->key[i]);
	  break;
	case integer32:
	  *(int32 *)tmp = conv_int32(*(int32 *)tmp_mtab->key[i]);
	  break;
	case floating32:
	  *(float32 *)tmp = conv_float32(*(float32 *)tmp_mtab->key[i]);
	  break;
	case floating64:
	  *(float64 *)tmp = conv_float64(*(float64 *)tmp_mtab->key[i]);
	  break;
	case string:
	  tmp = tmp_mtab->key[i];
	  break;
	}
      if (fwrite(tmp, db->key_size, 1, db->mfile) < 1)
	return ERR_M_FILE_WRITE;
      free(tmp_mtab->key[i]);
    }
  if (db->key_type != string)
    free(tmp);
  if (tmp_mtab->next_on_disc)
    {
      for(i=0; i <= tmp_mtab->range; i++)
	{
	  tmp32 = conv_int32(tmp_mtab->u[i].dtab);
	  if (fwrite(&tmp32, sizeof(int32), 1, db->mfile) < 1)
	    return ERR_M_FILE_WRITE;
	}
    }
  else
    {
      for (i=0; i <= tmp_mtab->range; ++i) 
	{
	  db->mtab = tmp_mtab->u[i].mtab;
	  if ((ret = write_mtabs(db)) != 0)
	    return ret;
	}
    }
  free((char *)tmp_mtab);
  db->mtab = NULL;
  return 0;
}



int16 db_rec_read(db_t *db, int32 db_key, void *rec[]) 
{
  int16 i, ret;
  
  if (fseek(db->dbfile, db_key, SEEK_SET) != 0)
    return ERR_DB_FILE_SEEK;

  for(i=0; i < db->num_fields; i++)
    {
      switch(db->fields[i].type)
	{
	case integer16:
	  {
	  int16 i16;
	  ret = fread(&i16, sizeof(int16), 1, db->dbfile);
	  *(int16 *)rec[i] = conv_int16(i16);
	  break;
	  }
	case integer32:
	  {
	  int32 i32;
	  ret = fread(&i32, sizeof(int32), 1, db->dbfile);
	  *(int32 *)rec[i] = conv_int32(i32);
	  break;
	  }
	case floating32:
	  {
	  float32 f32;
	  ret = fread(&f32, sizeof(f32), 1, db->dbfile);
	  *(float32 *)rec[i] = conv_float32(f32);
	  break;
	  }
	case floating64:
	  {
	  float64 f64;
	  ret = fread(&f64, sizeof(f64), 1, db->dbfile);
	  *(float64 *)rec[i] = conv_float64(f64);
	  break;
	  }
	case string:
	  ret = fread(rec[i], db->fields[i].size, 1, db->dbfile);
	  break;
	}
      if (ret == 0)
	return ERR_DB_FILE_READ;
    }
  return 0;
}


int16 db_rec_write(db_t *db, int32 db_key, void *rec[]) 
{
  int16  i, ret;
  
  if (fseek(db->dbfile, db_key, SEEK_SET) != 0)
    return ERR_DB_FILE_SEEK;
  
  for(i=0; i < db->num_fields; i++)
    {
      switch(db->fields[i].type)
	{
	case integer16:
	  {
	  int16	i16;
	  i16 = conv_int16(*(int16 *)rec[i]);
	  ret = fwrite(&i16, sizeof(int16), 1, db->dbfile);
	  break;
	  }
	case integer32:
	  {
	  int32 i32;
	  i32 = conv_int32(*(int32 *)rec[i]);
	  ret = fwrite(&i32, sizeof(int32), 1, db->dbfile);
	  break;
	  }
	case floating32:
	  {
	  float32 f32;
	  f32 = conv_float32(*(float32 *)rec[i]);
	  ret = fwrite(&f32, sizeof(float32), 1, db->dbfile);
	  break;
	  }
	case floating64:
	  {
	  float64 f64;
	  f64 = conv_float64(*(float64 *)rec[i]);
	  ret = fwrite(&f64, sizeof(float64), 1, db->dbfile);
	  break;
	  }
	case string:
	  ret = fwrite(rec[i], db->fields[i].size, 1, db->dbfile);
	  break;
	}
      if (ret == 0)
	return ERR_DB_FILE_WRITE;
    }
  return 0;
}




int16 db_compl_rec_read(db_t *db, d_tab_entry *entry, void *rec[]) 
{
  int16 ret;
  char  del;

  if ((ret = db_rec_read(db, entry->db_key, rec)) <= 0)
	return ret;
  if (fread(&del, 1, 1, db->dbfile) <= 0)
    return ERR_DB_FILE_READ;
  if (del == '0')
    entry->del = TRUE;
  else
    entry->del = FALSE;
  return 0;
}


int16 db_compl_rec_write(db_t *db, d_tab_entry *entry, void *rec[]) 
{
  int32                 tmp;
  int16 		ret;
  
  if (entry->db_key < 0L) 
    {
      /* new record to write */
      if (db->first_del_rec < 0L) 
	{
	  /* no permanently deleted records to overwrite */
	  if (fseek(db->dbfile, 0L, SEEK_END) != 0)
	    return ERR_DB_FILE_SEEK;
	  if ((entry->db_key = ftell(db->dbfile)) == -1)
	    return ERR_DB_FILE_SEEK;
	}
      else 
	{
	  /* overwrite next permanently deleted record */
	  if (fseek(db->dbfile, db->first_del_rec, SEEK_SET) != 0)
	    return ERR_DB_FILE_SEEK;
	  if (fread(&tmp, sizeof(int32), 1, db->dbfile) == 0)
	    return ERR_DB_FILE_READ;
	  entry->db_key = db->first_del_rec;
	  db->first_del_rec = conv_int32(tmp);	
	  /* next permanently deleted record */
	}
    }
  else 
    {
      if (fseek(db->dbfile, entry->db_key, SEEK_SET) != 0)
	return ERR_DB_FILE_SEEK;
    }
  if ((ret = db_rec_write(db, entry->db_key, rec)) <= 0)
    return ret;
  if (fwrite((entry->del == TRUE) ? "0" : "1", 1, 1, db->dbfile) <= 0)
    return ERR_DB_FILE_WRITE;
  return 0;
}



int16 read_db_stat(db_t *db) 
{
  int16     i,
            tmp16;
  int32     tmp32;
  char      line[line_len];
  
  rewind(db->cfgfile);
  if (fread(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->num_db_el = conv_int32(tmp32);
  if (fread(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->first_del_rec = conv_int32(tmp32);
  if (fread(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->num_db_eld = conv_int32(tmp32);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->m_tab_size = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->d_tab_size = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->key_size = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->key_type = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->record_size = conv_int16(tmp16);
  if (fread(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  db->num_fields = conv_int16(tmp16);
  if (fread(&db->name, FNAME, 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_READ;
  if ((db->fields = (field_desc_t *)malloc(db->num_fields * sizeof(field_desc_t))) == NULL)
    return ERR_NO_MEM;
  for (i=0; i < db->num_fields; i++)
    {
      fscanf(db->cfgfile, "%s\n", line);
      if (strlen(line) > 0) {
	if ((db->fields[i].name = (char *)malloc(strlen(line)+1)) == NULL)
	  return ERR_NO_MEM;
	strcpy(db->fields[i].name, line);
      }
      else
	db->fields[i].name = NULL;
      fscanf(db->cfgfile, "%s\n", line); db->fields[i].type = atoi(line);
      fscanf(db->cfgfile, "%s\n", line); db->fields[i].size = atoi(line);
      /* Note: don't know why, but the fscanf() was dumping core on
       * the Mips... ran ok after changed to "atoi()" - prv. 2 lines
       */
#if 0
      fscanf(db->cfgfile, "%d\n%d\n",
	&db->fields[i].type,
	&db->fields[i].size);
#endif
    }
  return 0;
}  /* read_db_status */


int16 write_db_stat(db_t *db) 
{
  int16 i, tmp16;
  int32 tmp32;

  rewind(db->cfgfile);
  tmp32 = conv_int32(db->num_db_el);
  if (fwrite(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp32 = conv_int32(db->first_del_rec);
  if (fwrite(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp32 = conv_int32(db->num_db_eld);
  if (fwrite(&tmp32, sizeof(int32), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->m_tab_size);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->d_tab_size);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->key_size);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->key_type);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->record_size);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  tmp16 = conv_int16(db->num_fields);
  if (fwrite(&tmp16, sizeof(int16), 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  if (fwrite(&db->name, FNAME, 1, db->cfgfile) < 1)
    return ERR_CFG_FILE_WRITE;
  for (i=0; i < db->num_fields; i++) 
    {
      fprintf(db->cfgfile, "%s\n%d\n%d\n", 
	      (db->fields[i].name == NULL) ? "" : db->fields[i].name, 
	      db->fields[i].type, 
	      db->fields[i].size);
    }
  return 0;
}  /* write_db_status */

