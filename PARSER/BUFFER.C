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
#include "buffer.h"

BNF_buffer *
BNF_buffer_create(FILE * stream)
{
BNF_buffer * buffer  = (BNF_buffer *) malloc(sizeof(BNF_buffer));

   buffer->stream    = stream;
   buffer->readin    = 0u;
   buffer->current   = 0u;
   buffer->done      = 0u;
   buffer->area      = STRcopy("");
   return buffer;
}

void
BNF_buffer_destroy(BNF_buffer * buffer)
{
   buffer->area   = STRfree(buffer->area  );
   free(buffer);
}

int
BNF_buffer_get(BNF_buffer * buffer)
{
   if( buffer->current < buffer->readin ) {
      return buffer->area  [buffer->current++ - buffer->done];
   }
   else {
   int c = fgetc(buffer->stream);
      if( c != EOF ) {
	 buffer->area   = STRput(buffer->area, c);
	 buffer->readin++;
	 buffer->current++;
      }
      return c;
   }
}

BNF_MARK
BNF_buffer_tell(BNF_buffer * buffer)
{
   return buffer->current;
}

int
BNF_buffer_seek(BNF_buffer * buffer, BNF_MARK mark)
{
   if( buffer->done > mark ) {
      return 0;
   }
   else {
      buffer->current = mark;
      return 1;
   }
}

void
BNF_buffer_cut(BNF_buffer * buffer, BNF_MARK mark)
{
   for( ; buffer->done < mark; buffer->done++ ) {
      STRpop(buffer->area  );
   }
   buffer->area   = STRnorm(buffer->area  );
}
