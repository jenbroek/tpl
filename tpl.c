/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "arg.h"
#include "config.h"

static void usage();

char *argv0;

void
usage()
{
	die("usage: %s [-v] [-d open_delim] [-D close_delim] [file]", argv0);
}

int
main(int argc, char *argv[])
{
	FILE *fp;

	char *buf, *evalbuf;
	char *ptr, *start, *end;

	int i, evallen;
	int open_delim_len;
	int close_delim_len;

	unsigned long len = 0;
	unsigned long size = BUFSIZ;

	ARGBEGIN {
	case 'v':
		fprintf(stderr, "%s-"VERSION"\n", argv0);
		return 0;
	case 'd':
		open_delim = EARGF(usage());
		break;
	case 'D':
		close_delim = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND

	if (!argv[0] || !strcmp(argv[0], "-"))
		fp = stdin;
	else if (!(fp = fopen(argv[0], "r")))
		die("%s: unable to open '%s' for reading:", argv0, argv[0]);

	buf = ecalloc(1, size);

	while ((i = fread(buf + len, 1, BUFSIZ, fp))) {
		len += i;
		if (BUFSIZ + len + 1 > size) {
			size += BUFSIZ;
			buf = erealloc(buf, size);
		}
	}

	buf[len] = '\0';
	ptr = buf;

	open_delim_len = strlen(open_delim);
	close_delim_len = strlen(close_delim);

	while ((start = strstr(ptr, open_delim))) {
		fwrite(ptr, 1, start - ptr, stdout);
		ptr = start + open_delim_len;

		if ((end = strstr(ptr, close_delim))) {
			evallen = end - ptr;
			evalbuf = ecalloc(1, evallen);

			memmove(evalbuf, ptr, evallen);
			fflush(stdout);
			system(evalbuf);

			free(evalbuf);
			ptr = end + close_delim_len;
		} else
			fwrite(open_delim, 1, open_delim_len, stdout);
	}

	fwrite(ptr, 1, strlen(ptr), stdout);
	free(buf);
	return 0;
}
