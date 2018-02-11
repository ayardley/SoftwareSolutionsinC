/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Dennis Jelcic.  Not derived from licensed software.
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
 *	str.c -- dynamic string data structure implementation
 */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "str.h"

static size_t
smear(size_t n)
{
	n |= n >> 1;	/* 1000 0000 0000 0000 -> 1100 0000 0000 0000 */
	n |= n >> 2;	/* 1100 0000 0000 0000 -> 1111 0000 0000 0000 */
	n |= n >> 4;	/* 1111 0000 0000 0000 -> 1111 1111 0000 0000 */
	n |= n >> 8;	/* 1111 1111 0000 0000 -> 1111 1111 1111 1111 */
	n |= n >> 16;	/* ... */
	return n;
}

static void
sizes(STR str, size_t *len, size_t *siz)
{
	size_t n;
	size_t m;
	size_t j;

	n = 0;
	m = 0;
	if (str) {
		for (n = 1; *(str - 1 + n); n <<= 1)
			;
		m = n >> 1;
		for (j = m >> 1; j > 0; j >>= 1) {
			if (*(str - 1 + (m | j))) {
				m |= j;
			}
		}
	}
	if (len) {
		*len = m;
	}
	if (siz) {
		*siz = n;
	}
}

STR
STRinit(int c0, ...)
{
	va_list ap;
	int c;
	size_t n;
	size_t b;
	STR s;

	/* count characters */
	n = 1;
	va_start(ap, c0);
	while (c = va_arg(ap, int)) {
		++n;
	}
	va_end(ap);
	/* allocate new STR object */
	b = smear(n) + 1;
	s = (STR)malloc(b);
	if (s) {
		/* copy characters into STR */
		n = 0;
		va_start(ap, c0);
		for (c = c0; c; c = va_arg(ap, int)) {
			s[n++] = c;
		}
		va_end(ap);
		/* pad with NULs */
		while (n < b) {
			s[n++] = '\0';
		}
	}
	return s;
}

size_t
STRlen(STR str)
{
	size_t len;

	sizes(str, &len, (size_t *)0);
	return len;
}

STR
STRcopy(char *s)
{
	STR p;
	size_t n;
	
	if (s == NULL) {
		s = "";
	}
	n = smear(strlen(s)) + 1;
	if (p = (STR)malloc(n)) {
		strncpy(p, s, n);
	}
	return p;
}

STR
STRncopy(char *s, size_t n)
{
	size_t size;
	STR p;

	if (s == NULL) {
		s = "";
	}
	size = smear(n) + 1;
	if (p = (STR)malloc(size)) {
		strncpy(p, s, size);
#if 0
/*
   The C standard states that strncpy() will pad the destination with NULs
   until <size> is reached, but if your implementation is broken, add this...
*/
		while (n < size) {
			p[n++] = '\0';
		}
#endif
	}
	return p;
}

STR
STRadd(STR str, STR cur, int c)
{
	size_t n;
	size_t m;
	size_t i;
	size_t d;
	STR p;

	sizes(str, &m, &n);
	if (n < 1) {
		n = 1;
	}
	d = (cur ? (size_t)(cur - str) : m);
	if ((m + 1) < n) {
		p = str;
	} else {
		n <<= 1;
		p = realloc(str, n);
		if (p) {		/* NULL the newly allocated part */
			memset((p + m), '\0', (n - m));
		}
	}
	if (p) {
		for (i = m; i > d; --i) {
			p[i] = p[i - 1];
		}
		p[d] = c;
	}
	return p;
}

STR
STRconc(STR str, ...)
{
	va_list ap;
	char *np;
	size_t b;
	size_t n;
	size_t len;
	size_t oldlen;
	size_t size;

	/* sum all string lengths */
	len = STRlen(str);
	va_start(ap, str);
	while (np = va_arg(ap, char *)) {
		len += strlen(np);
	}
	va_end(ap);
	/* make sure <str> has enough space for total length */
	sizes(str, &oldlen, &size);
	b = size;
	if (len >= size) {
		b = smear(len) + 1;
		str = (STR)realloc(str, b);
	}
	/* concatenate strings onto <str> */
	if (str) {
		n = strlen(str);
		va_start(ap, str);
		while (np = va_arg(ap, char *)) {
			while (*np) {
				str[n++] = *np++;
			}
		}
		va_end(ap);
		while (n < b) {		/* NULL the rest of the string */
			str[n++] = 0;
		}
	}
	return str;
}

int
STRrem(STR str, STR cur)
{
	int c;
	size_t n;
	size_t m;
	size_t i;
	size_t d;
	unsigned char *p;

	c = '\0';
	if (str && cur) {
		sizes(str, &m, &n);
		d = (size_t)(cur - str);
		p = (unsigned char *)str;
		c = p[d];
		for (i = d; i < m; ++i) {
			str[i] = str[i + 1];
		}
	}
	return c;
}

STR
STRnorm(STR str)
{
	size_t siz;

	if (str) {
		sizes(str, (size_t *)0, &siz);
		str = realloc(str, siz);
	}
	return str;
}

STR
STRfree(STR str)
{
	if (str) {
		free(str);
	}
	return (STR)0;
}

#ifdef TEST		/* use -DTEST to enable test fixture */
#include <stdio.h>

#define	SAFESTR(s)	((s)?(s):"<NULL>")

void
STR__test()
{
	static char buf[256];
	size_t len;
	size_t siz;
	int c;
	int i;
	int n;
	STR str;
	STR cur;

	printf("\nTesting NULL STR\n");
	str = (STR)0;
	n = STRlen(str);
	sizes(str, &len, &siz);
	printf("%2d: '%s' (%d) len=%u siz=%u\n",
		0, SAFESTR(str), n, (unsigned int)len, (unsigned int)siz);

	printf("\nTesting STRlen()\n");
	str = (STR)buf;
	for (i = 0; i < 20; ++i) {
		sprintf(buf, "%.*s", i, "abcdefghijklmnopqrstuvwxyz");
		n = STRlen(str);
		sizes(str, &len, &siz);
		printf("%2d: '%s' (%d) len=%u siz=%u\n",
			i, SAFESTR(str), n, (unsigned)len, (unsigned)siz);
	}

	printf("\nTesting STRput()\n");
	str = (STR)0;
	i = 'A';
	do {
		n = STRlen(str);
		sizes(str, &len, &siz);
		printf("%2d: '%s' (%d) len=%u siz=%u\n",
			i, SAFESTR(str), n, (unsigned)len, (unsigned)siz);
		str = STRput(str, i++);
	} while(i < 'T');
	str = STRfree(str);

	printf("\nTesting STRpush()\n");
	str = (STR)0;
	i = 'A';
	do {
		n = STRlen(str);
		sizes(str, &len, &siz);
		printf("%2d: '%s' (%d) len=%u siz=%u\n",
			i, SAFESTR(str), n, (unsigned)len, (unsigned)siz);
		str = STRpush(str, i++);
	} while(i < 'T');
	str = STRfree(str);

	printf("\nTesting STRcopy(), STRadd() and STRrem()\n");
	str = STRcopy("0123456789");
	i = 'A';
	do {
		n = STRlen(str);
		sizes(str, &len, &siz);
		printf("%2d: '%s' (%d) len=%u siz=%u\n",
			i, SAFESTR(str), n, (unsigned)len, (unsigned)siz);
		cur = strchr(str, '5');
		str = STRadd(str, cur, i++);
	} while(i < 'I');
	i = 'A';
	do {
		n = STRlen(str);
		sizes(str, &len, &siz);
		printf("%2d: '%s' (%d) len=%u siz=%u\n",
			i, SAFESTR(str), n, (unsigned)len, (unsigned)siz);
		cur = strchr(str, i++);
		c = STRrem(str, cur);
		printf("c=%d(%c)\n", c, c);
	} while(i < 'I');
	printf("final str=%p\n", str);
	str = STRnorm(str);
	printf("norm str=%p\n", str);
	str = STRfree(str);
	printf("free str=%p\n", str);
}
#endif /* TEST */
