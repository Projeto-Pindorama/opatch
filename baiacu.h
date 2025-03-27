/*
 * baiacu.h - Boilerplate code for building OpenBSD code on other UNIX
 */
/*
 * Copyright (C) 2025: Luiz Antônio Rangel (takusuman)
 *
 * SPDX-License-Identifier: BSD 2-Clause
 *
 * Partly based off "compat.h" header at Duncaen/lobase.
 */

#ifndef __OpenBSD__
/* A false pledge(). */
#ifndef HAVE_PLEDGE
#define pledge(x, y) 0
#else
int pledge(const char *, const char *[]);
#endif /* !HAVE_PLEDGE */
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

/* Functions to set the program name. */
#ifndef HAVE_SETPROGNAME
#include <string.h>
char *__progname = NULL;
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

#include <strings.h>
#if defined(__linux__) || defined(__NetBSD__) \
	|| (OpenBSD < 201405) || (FreeBSD < 201610)
/*
 * Emulate explicit_bzero() per making bzero() unoptimizable.
 * Reference:
 * https://www.daemonology.net/blog/2014-09-04-how-to-zero-a-buffer.html
 * It's not 100% guaranteed to work in any C99 compiler, but that's
 * what we've got today for implementing as simple as this.
 */
void *(const volatile *explicit_bzero) (void *, size_t);
explicit_bzero = bzero;
#else
void __dead explicit_bzero(void *b, size_t len);
#endif /* !__linux__, !NetBSD, OpenBSD < 5.5,
	* FreeBSD < 11.0 */

/* Some functions from BSD's stdio.h/stdlib.h. */
#if defined(__linux__) || (OpenBSD < 200411) \
	|| (FreeBSD < 200605) || (NetBSD < 201807) \
	|| !defined(__MidnightBSD__) \
	|| !defined(__DragonFly__) /* __linux__, OpenBSD < 3.6
				    * FreeBSD < 6.1, NetBSD < 8
				    * !MidnightBSD, !DragonFly */
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
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
#else /* !__linux__ */
#if (NetBSD > 201807)
/* Re-include <stdlib.h> with
 * _OPENBSD_SOURCE defined. */
#define _OPENBSD_SOURCE
#include <stdlib.h>
#endif /* !NetBSD */
long long strtonum(const char numstr[], long long minval,
		long long maxval, const char *errstrp[]);
#endif /* OpenBSD > 3.6, FreeBSD > 6.1, NetBSD > 8, MidnightBSD, DragonFly */


#ifdef __linux__
#include <stdio.h>
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

/* err.h functions. */
#define perror(mesg) \
	if (*mesg != NULL) fprintf(stderr, "%s: ", mesg); \
	fputs(strerror(errno), stderr); \
	fputc('\n', stderr);

#define __vwarncx(fmt, sep, ...) \
	fprintf(stderr, "%s: ", getprogname()); \
	if (fmt) { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		fputs(sep, stderr); \
	}

#define vwarnx(fmt, ...) __vwarncx(fmt, "\n", ##__VA_ARGS__)

#define vwarnc(wcode, fmt, ...) \
	__vwarncx(fmt, ": ", ##__VA_ARGS__); \
	fputs(strerror(wcode), stderr); \
	fputc('\n', stderr)

#define vwarn(fmt, ...) vwarnc(errno, fmt, ##__VA_ARGS__)

#define warnc(wcode, fmt, ...) vwarnc(wcode, fmt, ##__VA_ARGS__)

#define warnx(fmt, ...) vwarnx(fmt, ##__VA_ARGS__)

#define warn(fmt, ...) vwarn(fmt, ##__VA_ARGS__)

#define err(errv, fmt, ...) \
	warn(fmt, ##__VA_ARGS__); \
	exit(errv)

#define errc(errv, wcode, fmt, ...) \
	warnc(wcode, fmt, ##__VA_ARGS__); \
	exit(errv)

#define errx(errv, fmt, ...) \
	warnx(fmt, ##__VA_ARGS__); \
	exit(errv)
#else
#include <err.h>
#include <stdio.h>
char *fgetln(FILE *stream, size_t *len);
void __dead perror(const char *string);
void __dead warnc(int code, const char *fmt, ...);
void __dead warnx(const char *fmt, ...);
void __dead warn(const char *fmt, ...);
void __dead err(int eval, const char *fmt, ...);
void __dead errc(int eval, int code, const char *fmt, ...);
void __dead errx(int eval, const char *fmt, ...);
#endif /* !__linux__ */
