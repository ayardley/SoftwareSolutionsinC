/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Mayan Moudgill.  Not derived from licensed software.
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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "parser.h"

BNF_PARSER *
BNF_CREATE(FILE * stream)
{
BNF_PARSER * parser  = (BNF_PARSER *)  malloc(sizeof(BNF_PARSER));
   parser->buffer    = BNF_buffer_create(stream);
   parser->status    = BNF__MATCH;
   parser->match     = 1;
   parser->prev      = 0;
   return parser;
}

void
BNF_RESET(BNF_PARSER * parser)
{
   parser->status = BNF__MATCH;
   parser->match  = 1;
   parser->mark   = BNF_buffer_tell(parser->buffer);
}

void
BNF_DESTROY(BNF_PARSER * parser)
{
   BNF_buffer_destroy(parser->buffer);
   free(parser);
}

void
BNF__push(BNF_PARSER * parser)
{
BNF_PARSER * prev = (BNF_PARSER *) malloc(sizeof(BNF_PARSER));
   *prev          = *parser;
   parser->status = BNF__MATCH;
   parser->match  = 1;
   parser->mark   = BNF_buffer_tell(parser->buffer);
   parser->prev   = prev;
}

void
BNF__pop(BNF_PARSER * parser)
{
BNF_PARSER * prev = parser->prev;
   if( parser->status == BNF__FAIL ) {
      *parser = *prev;
      parser->status = BNF__FAIL;
   }
   else {
      *parser = *prev;
   }
   free(prev);
}

int
BNF__OR(BNF_PARSER * parser)
{
   if( parser->status != BNF__FAIL ) {
      return 1;
   }
   else {
      /*fprintf(stderr, "BNF_OR : %d\n", parser->mark);*/
      parser->status = BNF__MATCH;
      parser->match++;
      /* can't pop back out; probably going through a cut */
      if( BNF_buffer_seek(parser->buffer, parser->mark) == 0 )  {
	 parser->status = BNF__FAIL;
	 return 1;
      }
      return 0;
   }
}

void
BNF__TERMINAL(BNF_PARSER * parser, int c)
{
   if( parser->status == BNF__MATCH ) {
   int d = BNF_buffer_get(parser->buffer);
      /*fprintf(stderr, "BNF_TERMINAL: expected %c, got %c\n", c, d);*/
      if( c != d) {
	 parser->status = BNF__FAIL;
      }
   }
}

void
BNF__ALLOF(BNF_PARSER * parser, char * str)
{
   if( parser->status == BNF__MATCH ) {
   char * p;
      for ( p = str; *p; p++ ) {
         int d = BNF_buffer_get(parser->buffer);
	 if( d != *p ) {
	    parser->status = BNF__FAIL;
	    return;
	 }
      }
   }
}

int
BNF__ANY(BNF_PARSER * parser)
{
   if( parser->status == BNF__MATCH ) {
      return BNF_buffer_get(parser->buffer);
   }
   return 0;
}

int
BNF__ONEOF(BNF_PARSER * parser, char * str)
{
char * p;

   if( parser->status == BNF__MATCH ) {
   int d = BNF_buffer_get(parser->buffer);
      if( (p = strchr(str, d)) == 0) {
	 parser->status = BNF__FAIL;
	 return -1;
      }
      else {
	 return *p;
      }
   }
   return 0;
}

void
BNF_CUT(BNF_PARSER * parser)
{
   if( parser->status == BNF__MATCH ) {
      BNF_buffer_cut(parser->buffer, BNF_buffer_tell(parser->buffer));
   }
}
