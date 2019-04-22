/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "config.h"
#include "util.h"

static void run();
static void load(FILE *fp);
static void usage();

char *argv0;
static char *buf;

void
run()
{
	char *ptr = buf;
	char *begin, *end, *evalbuf;

	size_t evallen;
	size_t open_delim_len = strlen(open_delim);
	size_t close_delim_len = strlen(close_delim);

	while ((begin = strstr(ptr, open_delim))) {
		fwrite(ptr, begin - ptr, 1, stdout);
		ptr = begin + open_delim_len;

		if ((end = strstr(ptr, close_delim))) {
			evallen = end - ptr;
			evalbuf = ecalloc(1, evallen + 1);

			memmove(evalbuf, ptr, evallen);
			fflush(stdout);
			system(evalbuf);

			free(evalbuf);
			ptr = end + close_delim_len;
		} else {
			fwrite(open_delim, open_delim_len, 1, stdout);
		}
	}

	fwrite(ptr, strlen(ptr), 1, stdout);
}

void
load(FILE *fp)
{
	size_t len = 0;
	buf = ecalloc(1, BUFSIZ);

	while ((fread(buf + len, BUFSIZ, 1, fp))) {
		len += BUFSIZ;
		buf = erealloc(buf, len + BUFSIZ);
	}
}

void
usage()
{
	die("usage: %s [-v] [-o open_delim] [-c close_delim] [file]", argv0);
}

int
main(int argc, char *argv[])
{
	FILE *fp = stdin;

	ARGBEGIN {
	case 'v':
		fprintf(stderr, "%s-"VERSION"\n", argv0);
		return 0;
	case 'o':
		open_delim = EARGF(usage());
		break;
	case 'c':
		close_delim = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND

	if (argv[0] && !(fp = fopen(argv[0], "rb")))
		die("unable to open '%s' for reading:", argv[0]);

	load(fp);
	fclose(fp);

	run();
	free(buf);

	return 0;
}
