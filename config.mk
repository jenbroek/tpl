# tpl version
VERSION = 0.9

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# flags
CFLAGS  = -std=c99 -pedantic -Wall -Os \
          -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
LDFLAGS = -static
