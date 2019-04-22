# tpl version
VERSION = 0.3

# paths
PREFIX = /usr/local

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS  = -std=c99 -pedantic -Wall -Os $(CPPFLAGS)
LDFLAGS = -static

# compiler and linker
CC = cc
