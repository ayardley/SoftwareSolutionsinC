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

#define PB_BASIC_T /* Let callers avoid 2+ inclusions of this file */
/* some type defs used for portability reasons */
/* see your compiler's manual for appropriate data type */

/* compile with the appropriate setting of BIG_ENDIAN for your computer */
/*
 *	Settings for some typical systems are given in this table.
 *		BIG_ENDIAN = 0		BIG_ENDIAN = 1
 *		  80x86 (IBM PC)	  SPARC
 *		  i860			  MIPS
 *		  i960			  M680x0 (MAC, SUN3)
 *		  VAX			  M880x0
 *		  DECALPHA		  RS6000
 *		  DECSTATION		  HP_PA
 *		  NS320xx
 */
#ifndef BIG_ENDIAN
#error Sorry! You have to define BIG_ENDIAN to reflect your CPU type
#endif

#define int32                   long
#define int16                   short

#define float32                 float
#define float64                 double

#ifndef BOOLEAN
typedef int16			BOOLEAN;
#endif
#ifndef FALSE
#define FALSE			0
#endif
#ifndef TRUE
#define TRUE			1
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif
