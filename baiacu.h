/*
 * baiacu.h - Boilerplate code for building OpenBSD code on other UNIX
 */
/*
 * Copyright (C) 2025: Luiz Antônio Rangel (takusuman)
 *
 * SPDX-License-Identifier: BSD 2-Clause
 *
 * recallocarray() function and "MUL_NO_OVERFLOW" macro
 * completely pulled from lobase's libopenbsd/stdlib.
 * As per the copyright header of Duncaen/lobase's
 * "lib/libopenbsd/stdlib/reallocarray.c":
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net>
 *
 * SPDX-Licence-Identifier: ISC
 */

/* For operating system identification macro. */
#include <stdio.h>

#ifndef __OpenBSD__
/* Because of the recallocarray() static impl. */
#define OpenBSD 0
/* A false pledge() --- and now unveil(). */
#ifndef HAVE_PLEDGE
#define pledge(x, y) 0
#endif /* !HAVE_PLEDGE */
#ifndef HAVE_UNVEIL
#define unveil(x, y) 0
#endif /* !HAVE_UNVEIL */
#endif

#if defined(__OpenBSD__) || defined(__NetBSD__) \
	|| defined(__FreeBSD__)
#include <sys/param.h>
#endif /* !OpenBSD, !NetBSD, !FreeBSD */

#ifdef __GNUC__
#define __dead	__attribute__((__noreturn__))
#else
#define __dead /* Não há nada, nada há. */
#endif

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Functions to set the program name. */
#ifndef HAVE_SETPROGNAME
extern char *__progname;
#define setprogname(x) (__progname = strdup(x))
#define getprogname(x) __progname
#else
void __dead setprogname(const char *progname);
const char *getprogname(void);
#endif /* !HAVE_SETPROGNAME */

/* Get a directory entry name length. */
#ifndef _DIRENT_HAVE_NAMELEN
/*
 * Use d_reclen instead of PATH_MAX because
 * it is just the length of the directory
 * entry on the record.
 */
#define D_NAMLEN(e) strnlen((e)->d_name, (e)->d_reclen)
#else
#define D_NAMLEN(e) (e)->d_namlen
#endif

#if !defined(linux) || (defined(__GLIBC__) \
	&& (__GLIBC__ >= 2 && __GLIBC_MINOR__ > 25)) \
	|| !defined(__NetBSD__) || (OpenBSD > 201405) \
	|| (FreeBSD > 201610)
#include <strings.h>
#else
/*
 * Emulate explicit_bzero() per making bzero() unoptimizable.
 * Reference:
 * https://www.daemonology.net/blog/2014-09-04-how-to-zero-a-buffer.html
 * It's not 100% guaranteed to work in any C99 compiler, but that's
 * what we've got today for implementing as simple as this.
 */
static void (* volatile explicit_bzero)(void *, size_t) = bzero;
#endif /* __linux__, NetBSD, GLIBC < 2.25,
	* OpenBSD < 5.5, FreeBSD < 11.0 */

/* Some functions from BSD's stdio.h/stdlib.h. */
#if defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) \
	|| !defined(__DragonFly__) || defined(__OpenBSD__)
#if OpenBSD < 201704
#include <unistd.h>
/*
 * This is the same that sqrt(SIZE_MAX + 1), with s1 multiplied per s2
 * being smaller or equal to SIZE_MAX. It is used to prevent overflow
 * when multiplying these two numbers, the size of elements per the
 * number of elements itself, since the maximum value to the new size
 * of the array can be just (SIZE_MAX + 1).
 */
#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))
static inline void *recallocarray(void *ptr,
		size_t oldnmemb,
		size_t nmemb,
		size_t size) {
	size_t oldsize = 0,
		newsize = 0;
	void *newptr = NULL;

	if (ptr == NULL)
		return calloc(nmemb, size);

	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && (SIZE_MAX / nmemb) < size) {
		errno = ENOMEM;
		return NULL;
	}
	newsize = (nmemb * size);

	if ((oldnmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    oldnmemb > 0 && (SIZE_MAX / oldnmemb) < size) {
		errno = EINVAL;
		return NULL;
	}
	oldsize = (oldnmemb * size);

	/*
	 * Don't bother too much if we're shrinking just a bit,
	 * we do not shrink for series of small steps, oh well.
	 */
	if (newsize <= oldsize) {
		size_t d = (oldsize - newsize);

		if (d < (oldsize / 2) && d < getpagesize()) {
			memset(((char *)ptr + newsize), 0, d);
			return ptr;
		}
	}

	newptr = malloc(newsize);
	if (newptr == NULL)
		return NULL;

	if (newsize > oldsize) {
		memcpy(newptr, ptr, oldsize);
		memset((char *)newptr + oldsize, 0, newsize - oldsize);
	} else
		memcpy(newptr, ptr, newsize);

	explicit_bzero(ptr, oldsize);
	free(ptr);

	return newptr;
}
#endif
#endif /* OpenBSD >= 6.1, DragonFly */

#if !defined(__linux__) || (OpenBSD > 200411) \
	|| (FreeBSD > 200605) || (NetBSD > 201807) \
	|| defined(__MidnightBSD__) \
	|| defined(__DragonFly__) /* !__linux__ */
#if (NetBSD > 201807)
/* Re-include <stdlib.h> with
 * _OPENBSD_SOURCE defined. */
#define _OPENBSD_SOURCE
#endif /* !NetBSD */
#include <stdlib.h>
#else /* __linux__ */
static inline long long strtonum(const char numstr[],
		long long minval, long long maxval,
		const char *errstrp[]) {
	long long r = 0;
	char *invalid = NULL;
	*errstrp = NULL;
	errno = 0; /* Success */

	if (minval > maxval) {
		*errstrp = "minval cannot be larger than the maxval";
invalid:
		errno = EINVAL;
		r = 0;
		return r;
	}
	r = strtoll(numstr, &invalid, 10);
	if (*invalid != '\0' || numstr == invalid) {
		*errstrp = "invalid number string";
		goto invalid;
	}
	switch (errno) {
		case ERANGE:
			if (r == LLONG_MIN)
				*errstrp = "number is smaller than LLONG_MIN";
			else if (r == LLONG_MAX)
				*errstrp = "number is larger than LLONG_MAX";
			r = 0;
			break;
		default:
			if (r < minval || maxval < r) {
				errno = ERANGE;
				if (r < minval)
					*errstrp = "number is smaller than the minimal value";
				else if (maxval < r)
					*errstrp = "number is larger than the maximum value";
				r = 0;
			}
			break;
	}

	return r;
}
#endif /* OpenBSD < 3.6, FreeBSD < 6.1, NetBSD < 8, !MidnightBSD, !DragonFly */

#ifdef __linux__
static inline char *fgetln(FILE *restrict f, size_t *lenp) {
	/* Fail if we can't get access to stdio. */
	if (pledge("stdio", NULL) == -1)
		return NULL;

	char c = '\0',
	     *buf = NULL,
	     *nbuf = NULL;
	size_t bufsiz = 0,
	       nbufsiz = 0,
	       len = 0;

	if (buf == NULL) {
		bufsiz = BUFSIZ;
		if ((buf = malloc(bufsiz)) == NULL)
			return NULL; /* Can't allocate. */
	}

	for (; (c = getc(f)); len++) {
		if (c == EOF) return NULL;
		if ((len + 1) > bufsiz) {
			/* Increase the buffer size some more characters. */
			nbufsiz = (bufsiz + 256);
			nbuf = realloc(buf, nbufsiz);
			if (nbuf == NULL) {
				buf = NULL;
				len = -1; /* So it will be 0 later. */
				break;
			} else {
				buf = nbuf;
				bufsiz = nbufsiz;
			}
		}
		buf[len] = c;
		switch (c) {
			case '\n':
				break;
			default:
				continue;
		}
		break;
	}
	*lenp = len;
	*lenp += 1;

	return buf;
}
#endif /* !__linux__, *BSD */

#if defined(__linux__) || defined(__GLIBC__)
#include <stdarg.h>
/* err.h functions. */
#define __vwarncx(fmt, sep, ...) \
	fprintf(stderr, "%s: ", getprogname()); \
	if (fmt) { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		fputs(sep, stderr); \
	}

#define vwarnc(wcode, fmt, ap) __vwarncx(fmt, ": ", ap); \
	fputs(strerror(wcode), stderr); \
	fputc('\n', stderr)

static inline void warnc(int wcode, char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarnc(wcode, fmt, ap);
		va_end(ap);
}

static inline void errc(int ecode, int wcode, char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarnc(wcode, fmt, ap);
		va_end(ap);
		exit(ecode);
}
#endif
#if !defined(__linux__) || defined(__GLIBC__)
#include <err.h>
#else
#define vwarnx(fmt, ap) __vwarncx(fmt, "\n", ap)
#define vwarn(fmt, ap) vwarnc(errno, fmt, ap)
static inline void __dead warnx(char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarnx(fmt, ap);
		va_end(ap);
}

static inline void __dead warn(char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarn(fmt, ap);
		va_end(ap);
}

static inline void errx(int ecode, char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarnx(fmt, ap);
		va_end(ap);
		exit(ecode);
}

static inline void err(int ecode, char fmt[], ...) {
		va_list ap;
		va_start(ap, fmt);
		vwarn(fmt, ap);
		va_end(ap);
		exit(ecode);
}
#endif /* __linux__, !__GLIBC__, !*BSD */
