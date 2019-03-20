/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#include "config.h"

int
main(int argc, char *argv[])
{
	char *ptr, *start, *end;
	char *buf = ecalloc(1, BUFSIZ);

	int i;
	unsigned long len = 0;
	unsigned long size = BUFSIZ;

	int open_delim_len = strlen(open_delim);
	int close_delim_len = strlen(close_delim);

	while ((i = fread(buf + len, 1, BUFSIZ, stdin))) {
		len += i;
		if (BUFSIZ + len + 1 > size) {
			size += BUFSIZ;
			buf = erealloc(buf, size);
		}
	}

	buf[len] = '\0';
	ptr = buf;

	while ((start = strstr(ptr, open_delim))) {
		fwrite(ptr, 1, start - ptr, stdout);
		ptr = start + open_delim_len;

		if ((end = strstr(ptr, close_delim))) {
			int evallen = end - ptr;
			char *evalbuf = ecalloc(1, evallen);

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
