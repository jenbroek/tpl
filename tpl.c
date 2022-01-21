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
	int pipefd[2];
	char *sh;

	sigset_t old;
	struct sigaction oldint, oldquit;
	struct sigaction sa = { .sa_handler = SIG_IGN };

	if (!cmd)
		return;

	if (!(sh = getenv("TPL_SHELL")) && !(sh = getenv("SHELL")))
		die("%s: unable to determine shell", argv0);

	if (pipe(pipefd))
		die("%s: unable to create pipe:", argv0);

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

		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);

		execl(sh, basename(sh), NULL);
		_exit(127);
	default:
		close(pipefd[0]);
		if (write(pipefd[1], cmd, strlen(cmd) + 1) < 0)
			die("%s: unable to write to pipe:", argv0);
		close(pipefd[1]);
		waitpid(pid, NULL, 0);
		break;
	}

	sigaction(SIGINT, &oldint, NULL);
	sigaction(SIGQUIT, &oldquit, NULL);
	sigprocmask(SIG_SETMASK, &old, NULL);
}

void
run()
{
	int trim;
	char *begin, *end, *ptr = buf;

	size_t open_delim_len = strlen(open_delim);
	size_t close_delim_len = strlen(close_delim);
	size_t trim_chars_len = strlen(trim_chars);

	while ((begin = strstr(ptr, open_delim))) {
		fwrite(ptr, begin - ptr, 1, stdout);
		ptr = begin + open_delim_len;

		if ((end = strstr(ptr, close_delim))) {
			if (!strncmp(end - trim_chars_len, trim_chars, trim_chars_len)) {
				trim = 1;
				end -= trim_chars_len;
			} else {
				trim = 0;
			}

			memset(end, 0, 1);

			fflush(stdout);
			shell(ptr);

			ptr = end + close_delim_len + (trim ? trim_chars_len + 1 : 0);
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

	while ((fread(buf + len, 1, BUFSIZ, fp))) {
		len += BUFSIZ;
		buf = erealloc(buf, len + BUFSIZ);
		memset(buf + len, 0, BUFSIZ);
	}
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

	load(fp);
	fclose(fp);

	run();
	free(buf);

	return 0;
}
