# tpl version
VERSION = 0.5

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS  = -std=c99 -pedantic -Wall -Os $(CPPFLAGS)
LDFLAGS = -static

# compiler and linker
CC = cc
