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

#include "basic_t.h"

int16 conv_int16(int16 in)
{
#if BIG_ENDIAN
	register int16 out;

	out = (in >> 8) & 0x00FF;
	out |= (in << 8) & 0xFF00;

	return out;
#else
	return in;
#endif
}


int32 conv_int32(int32 in)
{
#if BIG_ENDIAN
	register int32	out;

	out = (in >> 24) & 0x000000FF;
	out |= (in >> 8) & 0x0000FF00;
	out |= (in << 8) & 0x00FF0000;
	out |= (in << 24) & 0xFF000000;

	return out;
#else
	return in;
#endif
}

float32	conv_float32(float32 in)
{
#if BIG_ENDIAN
	int32 		out;

	out = conv_int32(*(int32 *)&in);
	return *(float32 *) &out;
#else
	return in;
#endif
}

float64 conv_float64(float64 in)
{
#if BIG_ENDIAN
	int32		out[2];

	out[1] = conv_int32(*(int32 *) &in);
	out[0] = conv_int32(*(((int32 *) &in) + 1));

	return *(float64 *) &out[0];
#else
	return in;
#endif
}

