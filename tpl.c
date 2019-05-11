/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arg.h"
#include "config.h"
#include "util.h"

static void shell(const char *cmd);
static void run();
static void load(FILE *fp);
static void usage();

char *argv0;
static char *buf;

void
shell(const char *cmd)
{
	pid_t pid;
	char *sh;

	sigset_t old;
	struct sigaction oldint, oldquit;
	struct sigaction sa = { .sa_handler = SIG_IGN };

	if (!cmd)
		return;

	if (!(sh = getenv("TPL_SHELL")) && !(sh = getenv("SHELL")))
		die("unable to determine shell");

	sigaction(SIGINT, &sa, &oldint);
	sigaction(SIGQUIT, &sa, &oldquit);

	sigaddset(&sa.sa_mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &sa.sa_mask, &old);

	switch ((pid = fork())) {
	case -1:
		break;
	case 0:
		sigaction(SIGINT, &oldint, NULL);
		sigaction(SIGQUIT, &oldquit, NULL);
		sigprocmask(SIG_SETMASK, &old, NULL);

		execl(sh, basename(sh), "-c", cmd, NULL);
		_exit(127);
	default:
		while (waitpid(pid, NULL, 0) < 0 && errno == EINTR);
		break;
	}

	sigaction(SIGINT, &oldint, NULL);
	sigaction(SIGQUIT, &oldquit, NULL);
	sigprocmask(SIG_SETMASK, &old, NULL);
}

void
run()
{
	char *begin, *end;
	char *ptr = buf, *evalbuf;

	size_t evallen;
	size_t open_delim_len = strlen(open_delim);
	size_t close_delim_len = strlen(close_delim);

	while ((begin = strstr(ptr, open_delim))) {
		fwrite(ptr, begin - ptr, 1, stdout);
		ptr = begin + open_delim_len;

		if ((end = strstr(ptr, close_delim))) {
			evallen = end - ptr;
			evalbuf = ecalloc(1, evallen + 1);
			memcpy(evalbuf, ptr, evallen);

			fflush(stdout);
			shell(evalbuf);

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
