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

#include "buffer.h"

typedef struct BNF__parser BNF_PARSER;
typedef enum { BNF__MATCH, BNF__FAIL, BNF__ERROR } BNF__status;

struct BNF__parser {
   BNF_buffer *   buffer;  /* the buffer handles input and backtracking */
   BNF__status    status;  /* the status of the current level of parsing */
   int            match;   /* the sequence that matched or current sequence */ 
   BNF_MARK       mark;	   /* the mark of the beginning of the current level */
   BNF_PARSER *   prev;    /* a pointer to the previous level of parsing */
};

/* Creates a parser which reads from already opened stream */
BNF_PARSER *  BNF_CREATE(FILE * stream);
/* Resets a parser, so that it can start re-processing data from the current
 * point in the file;  used for error-recovery and for looping (probably in
 * combination with BNF_CUT()
 */
void	BNF_RESET(BNF_PARSER *);
/* Destroys parser object, deallocating storage; does not close stream */
void    BNF_DESTROY(BNF_PARSER *);

/* indicate beginning and end of a production; currently do nothing, but
 * MUST be used, for upward compatibility
 */
#define BNF_BEGIN(parser)			\
				do {		\
				   do {
#define BNF_END(parser)				\
				   } while(0);	\
				} while(0);

/* Indicates the beginning of an alternative sequence */
int BNF__OR(BNF_PARSER *);
#define BNF_OR(parser)				\
				   } while(0);	\
				   if( BNF__OR(parser) ) break; \
				   do {

/* The following 4 functions match characters from the input, if the parser
 * is attempting to match input i.e. it's status in BNF__MATCH.
 *    TERMINAL: matches a specific character.
 *    ALLOF: matches a sequence of characters.
 *    ONEOF: matches one of a set of characters.
 *    ANY: matches any single character.
 */
void    BNF__TERMINAL(BNF_PARSER *, int);
#define BNF_TERMINAL(parser, tomatch) {		\
				      BNF__TERMINAL(parser, tomatch); \
				      if( (parser)->status != BNF__MATCH ) \
					 break;	\
				      }
void    BNF__ALLOF(BNF_PARSER *, char *);
#define BNF_ALLOF(parser, tomatch)    {		\
				      BNF__ALLOF(parser, tomatch); \
				      if( (parser)->status != BNF__MATCH ) \
					 break;	\
				      }

int     BNF__ONEOF(BNF_PARSER *, char *);
#define BNF_ONEOF(parser, tomatch, toset) {	\
				      toset = BNF__ONEOF(parser, tomatch); \
				      if( (parser)->status != BNF__MATCH ) \
					 break;	\
				      }
int     BNF__ANY(BNF_PARSER *);
#define BNF_ANY(parser, toset) 	      {	\
				      toset = BNF__ANY(parser);	\
				      if( (parser)->status != BNF__MATCH ) \
					 break;	\
				      }

/* BNF_NON()'s 2nd argument is a call to the function containing the
 * production for the non-terminal. It is invoked if the parser is
 * attempting to match input. i.e. if the parsers status is BNF__MATCH
 */
void    BNF__push(BNF_PARSER *);
void    BNF__pop(BNF_PARSER *);
#define BNF_NON(parser, call)	      {	\
				      BNF__push(parser); \
				      call;	\
				      BNF__pop(parser); \
				      if( (parser)->status != BNF__MATCH ) \
				         break;	\
				      }

/* The following functions are utility functions
 *    BNF_ISMATCH: checks whether the production has succeeded
 *    BNF_ISFAIL: checks whether the production has failed
 *    BNF_ACTION: returns 0 if the production failed, otherwise returns
 *                the index of the succeeding alternative.
 *    BNF_FAIL: forces the status to be failed.
 *    BNF_MATCH: forces the status to be matched.
 */
#define BNF_ISMATCH(parser)	((parser)->status == BNF__MATCH )
#define BNF_ISFAIL(parser)	((parser)->status != BNF__MATCH )
#define BNF_ACTION(parser)	(BNF_ISFAIL(parser)?0:(parser)->match)
#define BNF_FAIL(parser)	{ ((parser)->status = BNF__FAIL); break; }
#define BNF_MATCH(parser)	((parser)->status = BNF__MATCH)

/* BNF_CUT is used for optimizing memory usage. It is used when it is
 * KNOWN that the parser will attempt to back up through this point
 * on an error. At that point there is no need to save the prior input
 * and space can be saved by discarding it.
 */
void    BNF_CUT(BNF_PARSER *);
