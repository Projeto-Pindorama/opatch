/*
 * ignorance.h - Boilerplate code for building OpenBSD code on other UNIX
 */
/*
 * Copyright (C) 2025: Luiz Antônio Rangel (takusuman)
 *
 * SPDX-License-Identifier: Unlicense
 *
 * Partly based off "compat.h" header at Duncaen/lobase.
 */

#ifndef OpenBSD
#ifdef __GNUC__
#define __dead	__attribute__((__noreturn__))
#else
#define __dead /* Não há nada, nada há. */
#endif

/* A false pledge(). */
#ifndef HAVE_PLEDGE
#define pledge(x, y) 0
#else
int pledge(const char *, const char *[]);
#endif /* !HAVE_PLEDGE */

/* Functions to set the program name. */
#ifndef HAVE_SETPROGNAME
#include <string.h>
char *__progname = NULL;
#define setprogname(x) (__progname = strdup(x))
#define getprogname(x) __progname
#else
const char *getprogname(void);
void __dead setprogname(const char *progname);
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

/* fgetln() implementation. */
#include <stdio.h>
#include <stdlib.h>
inline char *fgetln(FILE *restrict f, size_t *lenp) {
	/* Fail if we can't get access to stdio. */
	if (pledge("stdio", NULL) == -1)
		return NULL;

	char c = '\0',
	     *buf = NULL;
	size_t bufsiz = 0,
	       len = 0;

	if (buf == NULL) {
		bufsiz = BUFSIZ;
		buf = malloc(bufsiz);
		if (buf == NULL) return NULL; /* Can't allocate */
	}

	for (; (c = getc(f)); len++) {
		if (c == EOF) return NULL;

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

#include <errno.h>
#include <stdarg.h>
/* err.h functions. */
#define __vwarncx(fmt, sep, ...) \
	fprintf(stderr, "%s: ", __progname); \
	if (fmt) { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		fprintf(stderr, "%s", sep); \
	}

#define vwarnx(fmt, ...) __vwarncx(fmt, "\n", ##__VA_ARGS__)

#define vwarnc(wcode, fmt, ...) \
	__vwarncx(fmt, ": ", ##__VA_ARGS__); \
	fputs(strerror(wcode), stderr); \
	fputc('\n', stderr)

#define vwarn(fmt, ...) vwarnc(errno, fmt, ##__VA_ARGS__)

#define warnc(wcode, fmt, ...) vwarnc(wcode, fmt, ##__VA_ARGS__);

#define warnx(fmt, ...) vwarnx(fmt, ##__VA_ARGS__);

#define warn(fmt, ...) vwarn(fmt, ##__VA_ARGS__);
#endif
