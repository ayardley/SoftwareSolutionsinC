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

#define PB_BTREE_T 1
#ifndef PB_BASIC_T
#include "basic_t.h"
#endif
/* Let would-be includers know this is already active. */

#define FNAME	   9   /* length of filename without extension incl. terminating \0 */
#define EXT_SIZE   4   /* size of filename-extension incl. terminating \0 */

/* error codes returned by functions */
#define	ERR_NO_MEM			 1
#define ERR_D_FILE_OPEN			10
#define ERR_D_FILE_SEEK			11
#define ERR_D_FILE_READ			12
#define ERR_D_FILE_WRITE		13
#define ERR_D_FILE_REMOVE		14
#define ERR_CFG_FILE_OPEN		20
#define ERR_CFG_FILE_SEEK		21
#define ERR_CFG_FILE_READ		22
#define ERR_CFG_FILE_WRITE		23
#define ERR_DB_FILE_OPEN		30
#define ERR_DB_FILE_SEEK		31
#define ERR_DB_FILE_READ		32
#define ERR_DB_FILE_WRITE		33
#define ERR_DB_FILE_DATA		34
#define ERR_M_FILE_OPEN			40
#define ERR_M_FILE_SEEK			41
#define ERR_M_FILE_READ			42
#define ERR_M_FILE_WRITE		43
#define ERR_TMP_FILE_OPEN		50
#define ERR_TMP_FILE_SEEK		51
#define ERR_TMP_FILE_READ		52
#define ERR_TMP_FILE_WRITE		53
#define ERR_TMP_FILE_RENAME		54
#define ERR_DDL_FILE_OPEN               60
#define ERR_DDL_FILE_READ               61

#define ERR_REC_EXISTS		       100
#define ERR_REC_DOES_NOT_EXIST         101
#define ERR_BUILD_INDEX		       102
#define ERR_REBUILD		       103
#define ERR_DELETE		       104
#define ERR_UNDELETE		       105
#define ERR_FATAL		      9999

#define TMP_FN			"db.tmp"

#define CFG_EXT		        ".cfg"
#define DB_EXT			".dbf"
#define M_EXT		        ".mdx"	   /* memory part of tree */
#define D_EXT			".ddx"	   /* disk part of tree */

#define WILD_CARD_SIGN		'?'
#define JOKER_SIGN		'*'


#define max_mem			59	   /* # of entries in memory node, must be odd */
#define max_disc		145	   /* # of entries in disk leaf */


#define  string 0 
#define  integer16 1 
#define  integer32 2 
#define  floating32 3 
#define  floating64 4


typedef struct d_tab_entry 
{
  void          *key;       /*  pointer to key */
  BOOLEAN	 del;	    /*  used to mark as deleted	*/
  int32		 db_key;    /*  offset in db-file */
} d_tab_entry;


typedef struct m_tab_t 
{
  BOOLEAN	 next_on_disc;   /*  is TRUE when next node is disk node */
  int16		 range;          /*  number of entries in this node */
  union 
    {
      int32	       dtab;    /*  array of offsets in disk-index file */
      struct m_tab_t  *mtab;    /*  array of pointers child nodes in mem */
    } u[max_mem+1];
  void	        *key[max_mem];   /*  array of pointer to keys */
} m_tab_t;


typedef struct d_tab_t 
{     
  int16		 range;          /*  number of entries in disk node */
  int32	   	 ownkey;	 /*  own position in disk-index file */
  int32	   	 next_dtab;      /*  position of next node in disk-index file */
  int32		 prev_dtab;      /*  position of previous node in disk-index file */
  struct d_tab_entry	tab[max_disc]; /* array of keys, file offsets and del flags */
} d_tab_t;


typedef struct field_desc_t
{
  char           *name;           /* name of field */
  int16          type;            /* datatype of field */
  int16          size;            /* length of field, in bytes */
} field_desc_t;


typedef struct db_t 
{
  struct m_tab_t       *mtab;
  struct d_tab_t       *dtab;
  FILE		       *mfile,
                       *dfile,
                       *dbfile,
                       *cfgfile;
  int16			dindex, tindex; /* dindex=current, tindex=traversal */
  BOOLEAN		changed;
  int32			curr_dkey;
  int16			pos,		  /* position of first wildcard or joker */
                        match_type;	  /* type of matching */
  char		       *match_string;
  void                 *upper_bound;      /* upper bound for range searches */
  void                 *lower_bound;      /* lower bound for range searches */
  BOOLEAN		case_sensitive;	  /* casesensitive matching */
  char		        name[FNAME];	  /* name of db without extension */
  int32 	        num_db_el;  	  /* # of elements in database */
  int32		        first_del_rec;    /* first deleted record */
  int32 	        num_db_eld; 	  /* # of deleted elements in database */
  int16                 m_tab_size;      /*  size of mem-index record */
  int16                 d_tab_size;      /*  size of disk-index record */
  int16                 key_size;        /*  size of key in bytes/chars */
  int16                 key_type;        /*  datatype of key */
  int16                 record_size;     /*  size of db_rec in bytes */
  int16                 glob_rec_size;   /*  size of global_rec in bytes */
  int16                 num_fields;      /*  number of fields in record */
  field_desc_t         *fields;          /*  pointer to array of field-descriptors */
} db_t;

