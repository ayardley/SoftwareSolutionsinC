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
 *	lst.c -- dynamic list data structure implementation
 */
#include <stdlib.h>
#include <stdarg.h>
#include "lst.h"

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
sizes(LST lst, size_t *len, size_t *siz)
{
	size_t n;
	size_t m;
	size_t j;

	n = 0;
	m = 0;
	if (lst) {
		for (n = 1; *(lst - 1 + n); n <<= 1)
			;
		m = n >> 1;
		for (j = m >> 1; j > 0; j >>= 1) {
			if (*(lst - 1 + (m | j))) {
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

LST
LSTinit(void *p, ...)
{
	va_list ap;
	void *np;
	size_t n;
	size_t b;
	LST lst;

	n = 1;
	va_start(ap, p);
	while (np = va_arg(ap, void *)) {
		++n;
	}
	va_end(ap);
	b = smear(n) + 1;
	lst = (LST)malloc(b * sizeof(void *));
	if (lst) {
		n = 0;
		va_start(ap, p);
		for (np = p; np; np = va_arg(ap, void *)) {
			lst[n++] = np;
		}
		va_end(ap);
		while (n < b) {
			lst[n++] = (void *)0;
		}
	}
	return lst;
}

size_t
LSTlen(LST lst)
{
	size_t len;

	sizes(lst, &len, (size_t *)0);
	return len;
}

LST
LSTadd(LST lst, LST cur, void *p)
{
	size_t n;
	size_t m;
	size_t i;
	size_t d;
	LST lp;

	sizes(lst, &m, &n);
	if (n < 1) {
		n = 1;
	}
	d = (cur ? (size_t)(cur - lst) : m);
	if ((m + 1) < n) {
		lp = lst;
	} else {
		n <<= 1;
		lp = realloc(lst, n * sizeof(void *));
		if (lp) {
			lst = (lp + m);
			i = (n - m);
			while (i-- > 0) {
				*lst++ = (void *)0;
			}
		}
	}
	if (lp) {
		for (i = m; i > d; --i) {
			lp[i] = lp[i - 1];
		}
		lp[d] = p;
	}
	return lp;
}

void *
LSTrem(LST lst, LST cur)
{
	void *p;
	size_t n;
	size_t m;
	size_t i;
	size_t d;

	p = (void *)0;
	if (lst && cur) {
		sizes (lst, &m, &n);
		d = (size_t)(cur - lst);
		p = lst[d];
		for (i = d; i < m; ++i) {
			lst[i] = lst[i + 1];
		}
	}
	return p;
}

LST
LSTnorm(LST lst)
{
	size_t siz;

	if (lst) {
		sizes (lst, (size_t *)0, &siz);
		lst = realloc(lst, siz * sizeof(void *));
	}
	return lst;
}

LST
LSTfree(LST lst)
{
	if (lst) {
		free(lst);
	}
	return (LST)0;
}

#ifdef TEST		/* use -DTEST to enable test fixture */
#include <stdio.h>

static void
LST__print(LST lst)
{
	char *pfx = "";
	char *p;

	if (lst) {
		printf("(");
		while (p = (char *)*lst++) {
			printf("%s'%s'", pfx, p);
			pfx = ",";
		}
		printf(")\n");
	} else {
		printf("<NULL>\n");
	}
}

void
LST__test()
{
	static char *number[] = {
"zero","one","two","three","four","five","six","seven","eight","nine",(char
*)0
	};
	size_t len;
	size_t siz;
	char *p;
	int i;
	int n;
	LST lst;

	printf("\nTesting NULL LST\n");
	lst = (LST)0;
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		0, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	printf("\nTesting LSTput() and LSTlen()\n");
	lst = (LST)0;
	for (i = 0; i < 10; ++i) {
		lst = LSTput(lst, (void *)number[i]);
		n = LSTlen(lst);
		sizes(lst, &len, &siz);
		printf("%d: (%d) len=%u siz=%u lst=",
			i, n, (unsigned int)len, (unsigned int)siz);
		LST__print(lst);
	}
	lst = LSTfree(lst);

	printf("\nTesting LSTpush()\n");
	lst = (LST)0;
	for (i = 0; i < 10; ++i) {
		lst = LSTpush(lst, (void *)number[i]);
		n = LSTlen(lst);
		sizes(lst, &len, &siz);
		printf("%d: (%d) len=%u siz=%u lst=",
			i, n, (unsigned int)len, (unsigned int)siz);
		LST__print(lst);
	}
#if 0
	lst = LSTfree(lst);
#endif

	printf("\nTesting LSTadd() and LSTrem()\n");

	p = "NEW";
	i = 3;
	LSTadd(lst, lst + 3, (void *)p);
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		i, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	if ((char *)LSTrem(lst, lst + 3) == p) {
		n = LSTlen(lst);
		sizes(lst, &len, &siz);
		printf("%d: (%d) len=%u siz=%u lst=",
			i, n, (unsigned int)len, (unsigned int)siz);
		LST__print(lst);
	} else {
		printf("LSTrem did not return '%s'@%p\n", p, p);
	}

	i = 7;
	LSTrem(lst, lst+7);
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		i, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	i = 0;
	LSTrem(lst, lst+0);
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		i, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	i = 4;
	LSTrem(lst, lst+4);
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		i, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	i = -1;
	while (LSTpop(lst))
		;
	n = LSTlen(lst);
	sizes(lst, &len, &siz);
	printf("%d: (%d) len=%u siz=%u lst=",
		i, n, (unsigned int)len, (unsigned int)siz);
	LST__print(lst);

	printf("final lst=%p\n", lst);
	lst = LSTnorm(lst);
	printf("norm lst=%p\n", lst);
	lst = LSTfree(lst);
	printf("free lst=%p\n", lst);
}
#endif /* TEST */
