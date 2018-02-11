/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Douglas A. Gwyn.  Not derived from licensed software.
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

/*
	tststr -- tests 3-phase string search algorithms for correctness

	Usage:	$ tststr

	If there are any incorrect results, they will be displayed on the
	standard error output and the program will eventually return a
	"failure" status indication to the invoking environment.  It is
	possible for an incorrect algorithm to enter a nonterminating loop;
	total execution time should be no more than a few seconds on even
	the slowest systems, so if it takes much longer you may assume that
	a nonterminating loop has occurred.  All the algorithms supplied
	have passed these tests on a wide variety of computer systems.
 */
#ifndef	LOWAL
#define	LOWAL	1		/* 0 to also test strstr(), 1 otherwise */
#endif

#include	<stdio.h>
#include	<stdlib.h>
#if	LOWAL == 0
#include	<string.h>
#endif

extern void	ni_setpat( const char * );
extern char	*ni_search( const char * );
extern void	ni_clrpat( void );

extern void	bf_setpat( const char * );
extern char	*bf_search( const char * );
extern void	bf_clrpat( void );

extern void	kmp_setpat( const char * );
extern char	*kmp_search( const char * );
extern void	kmp_clrpat( void );

extern void	bm_setpat( const char * );
extern char	*bm_search( const char * );
extern void	bm_clrpat( void );

extern void	rk_setpat( const char * );
extern char	*rk_search( const char * );
extern void	rk_clrpat( void );

extern void	qs_setpat( const char * );
extern char	*qs_search( const char * );
extern void	qs_clrpat( void );

struct afp				/* algorithm function pointers */
	{
	void	(*setpat)( const char * );	/* set search pattern */
	char	*(*search)( const char * );	/* find pattern in text */
	void	(*clrpat)( void ); 		/* finished with pattern */
	}	alg[] =
	{
	NULL,		NULL,		NULL,
	ni_setpat,	ni_search,	ni_clrpat,
	bf_setpat,	bf_search,	bf_clrpat,
	kmp_setpat,	kmp_search,	kmp_clrpat,
	bm_setpat,	bm_search,	bm_clrpat,
	rk_setpat,	rk_search,	rk_clrpat,
	qs_setpat,	qs_search,	qs_clrpat,
	};

#define	MAXAL	(sizeof alg / sizeof(struct afp))	/* max. al + 1 */

#define	strsetpat	alg[al].setpat
#define	strsearch	alg[al].search
#define	strclrpat	alg[al].clrpat

struct tc				/* test case */
	{
	const char	*pattern;	/* search pattern */
	const char	*text;		/* text to be searched */
	int		loc; 		/* index in text, or -1 if none */
	}	data[] =
	{
	/* assorted test cases intended to exercise special cases: */
	"",		"",			0,
	"",		"a",			0,
	"b",		"",			-1,
	"c",		"c",			0,
	"defghi",	"defghk",		-1,
	"jklmno",	"jklmno",		0,
	"tuv",		"pqrstuv",		4,
	"yz",		"wxyz",			2,
	"aa",		"",			-1,
	"bb",		"bb",			0,
	"dd",		"cdde",			1,
	"hh",		"fghh",			2,
	"iki",		"iji",			-1,
	"lml",		"lmlm",			0,
	"ono",		"nono",			1,
	"pqr",		"pqpqrs",		2,
	"tut",		"tuuututttuv",		4,
	"xxy",		"wxxyxxz",		1,
	"ama",		"bananrama",		6,
	"xyzzy",	"xyzzy",		0,
	"xyzzy",	"zyzzy",		-1,
	"ababa",	"abacabadababababa",	8,
	"ababb",	"abacabadababababb",	12,
	"110111",	"110110111011110",	3,
	"1011110",	"110110111011110",	8,
	"abcd",		"xyzabc",		-1,
	"abcd",		"abc",			-1,
	" ",		"foo bar",		3,
	"",		NULL,		-1,	/* questionable case */

	/* examples from various versions of Sedgewick's "Algorithms": */
	"STING",	"A STRING SEARCHING EXAMPLE CONSISTING OF ...",	32,
	"10010111",	"100111010010010010010111000111",	16,
	"10100111",	"100111010010100010100111000111",	16,
	"10100111",	"1010100111",		2,

	/* examples from Dromey's "How to Solve It by Computer":
	"SENTENCE",	"THIS IS A SENTENCE OF TEXT",	10,
	"SENTENCE",	"THIS IS A RATHER SLOW ATTACK AT TEXT...",	-1,
	"SEPTEMBER",	"LOOKING FOR SEPTEMBER SHOWER",	12,
	"abcabdabc",	"abcabcabdabc",		3,
	"abcabcacab",	"abcabdabcabcacababc",	6,

	/* examples from Wirth's "Algorithms & Data Structures": */
	"Hooligan",	"Hoola-Hoola girls like Hooligans.",	23,
	"ABCE",		"ABCDABCE",		4,
	"AAAAAB",	"AAAAACAAAAAB",		6,
	"ABCABC",	"ABCABDABCABC",		6,
	"ABCDEF",	"ABCDEABCDEF",		5,
	"ABCDEA",	"ABCDEFABCDEA",		6,
	"ABCDEG",	"ABCDEFABCDEG",		6,

	/* examples from Cormen, Leiserson, & Rivest's
			 "Introduction to Algorithms": */
        "abaa",		"abcabaabcabac",	3,
	"0001",		"000010001010001",	1,
	"ababaca",	"abababacaba",		2,
	"aabab",	"aaababaabaababaab",	1,
	"ababbabbababbababbabb",	"ababbababbabbababbababbabb",	5,
	"ababaca",	"bacbababaabcbab",	-1,
	"reminiscence",	"itten_notice_that...",	-1,
	"reminiscence",	"ution_in_the_treatment_of...",	-1,
	"reminiscence",	"olden_fleece_of...",	-1,
	};

#define	MAXTRY	(sizeof data / sizeof(struct tc))	/* max. data + 1 */

int
main( int argc, char *argv[] )
	{
	register unsigned	pass;	/* 0, 1, 2 */
	register unsigned	peek;	/* 0, 1, 2 */
	register int		try;	/* indexes test case */
	register int		al;	/* indexes strstr() algorithm */
	register char		*rp;	/* return from strsearch() */
	register int		ri;	/* return index in text, or -1 */
	int			status = EXIT_SUCCESS;	/* exit status */

	/* For each strstr() algorithm: */
	for ( al = LOWAL; al < MAXAL; ++al )
		/* Multiple test cases: */
		for ( try = 0; try < MAXTRY; ++try )
			/* Multiple passes to check cleanup etc.: */
			for ( pass = 0; pass < 3; ++pass )
				{
#if	LOWAL == 0
				if ( al != 0 )
#endif
					strsetpat( data[try].pattern );

				for ( peek = 0; peek < 3; ++peek )
					{
					rp =
#if	LOWAL == 0
						al == 0 ?
						strstr( data[try].text,
							data[try].pattern
						      ) :
#endif
						strsearch( data[try].text );

					if ( rp == NULL )
						ri = -1;
					else
						ri = rp - data[try].text;

					if ( ri != data[try].loc )
						{
						fprintf( stderr,
			"tststr: strstr[%d](%s,%s) was %d, should be %d\n",
							 al,
							 data[try].text,
							 data[try].pattern,
							 ri,
							 data[try].loc
						       );
						status = EXIT_FAILURE;
						}						
					}

#if	LOWAL == 0
				if ( al != 0 )
#endif
					strclrpat();
				}

	return status;
	}
