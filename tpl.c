/* See LICENSE file for copyright and license details. */
#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arg.h"
#include "config.h"
#include "util.h"

static void shell(const char *cmd);
static void run(FILE *fp);
static int load(char **buf, FILE *fp, size_t len);
static void usage();

char *argv0;
static char *sh;

void
shell(const char *cmd)
{
	pid_t pid;
	int pipefd[2];

	if (!cmd)
		return;

	if (pipe(pipefd))
		die("%s: unable to create pipe:", argv0);

	switch ((pid = fork())) {
	case -1:
		break;
	case 0:
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);

		execlp(sh, sh, NULL);
		_exit(127);
	default:
		close(pipefd[0]);
		if (write(pipefd[1], cmd, strlen(cmd) + 1) < 0)
			die("%s: unable to write to pipe:", argv0);
		close(pipefd[1]);
		waitpid(pid, NULL, 0);
		break;
	}
}

void
run(FILE *fp)
{
	int trim = 0, eval = 0;
	char *buf = NULL, *ptr, *begin, *end;

	size_t len = 0;
	ptrdiff_t offset = 0;

	size_t open_delim_len = strlen(open_delim);
	size_t close_delim_len = strlen(close_delim);
	size_t trim_chars_len = strlen(trim_chars);

	while (load(&buf, fp, len)) {
		ptr = buf + offset;

		if (eval) {
			if ((end = strstr(ptr, close_delim))) {
				eval = 0;
				goto close_delim_found;
			}

			goto load_next;
		}

		while ((begin = strstr(ptr, open_delim))) {
			fwrite(ptr, begin - ptr, 1, stdout);
			ptr = begin + open_delim_len;

			if ((end = strstr(ptr, close_delim))) {
close_delim_found:
				if (!strncmp(end - trim_chars_len, trim_chars, trim_chars_len)) {
					trim = trim_chars_len + 1;
					end -= trim_chars_len;
				} else {
					trim = 0;
				}

				memset(end, '\0', 1);

				fflush(stdout);
				shell(ptr);

				ptr = end + close_delim_len + trim;
			} else {
				eval = 1;
				goto load_next;
			}
		}

		if (!strncmp(ptr+strlen(ptr)-1, open_delim, 1)) {
load_next:
			offset = ptr - buf;
			len += BUFSIZ;
		} else {
			fwrite(ptr, strlen(ptr), 1, stdout);
			memset(buf, 0, ptr-buf);
			offset = 0;
			len = 0;
		}
	}

	free(buf);
}

int
load(char **buf, FILE *fp, size_t len)
{
	*buf = erealloc(*buf, len + BUFSIZ);
	memset(*buf + len, 0, BUFSIZ);
	return fread(*buf + len, 1, BUFSIZ, fp);
}

void
usage()
{
	die("usage: %s [-v] [-o OPEN_DELIM] [-c CLOSE_DELIM] [-t TRIM_CHARS] [FILE]", argv0);
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
	case 't':
		trim_chars = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND

	if (argv[0] && !(fp = fopen(argv[0], "rb")))
		die("%s: unable to open '%s' for reading:", argv0, argv[0]);

	if (!(sh = getenv("TPL_SHELL")) && !(sh = getenv("SHELL")))
		die("%s: unable to determine shell", argv0);

	run(fp);
	fclose(fp);

	return 0;
}
