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

/* length of char* returned by packing functions is same as size of packed */
/* data type, no trailing \0 are appended to the string!!! */
extern void pack_int16(int16 i, char *dest);
extern void pack_int32(int32 l, char *dest);
extern void pack_float32(float32 f, char *dest);
extern void pack_float64(float64 d, char *dest);

extern int16 unpack_int16(char *cp);
extern int32 unpack_int32(char *cp);
extern float32 unpack_float32(char *cp);
extern float64 unpack_float64(char *cp);
