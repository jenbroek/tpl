# tpl - shell templating utility
# See LICENSE file for copyright and license details.

.POSIX:
.SUFFIXES: .c .o

include config.mk

SRC = tpl.c util.c
OBJ = $(SRC:.c=.o)

all: options tpl

options:
	@echo tpl build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): arg.h config.h config.mk util.h

config.h:
	cp config.def.h $@

tpl: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f tpl $(OBJ) tpl-$(VERSION).tar.gz

dist: clean
	mkdir -p tpl-$(VERSION)
	cp -R LICENSE Makefile README.md arg.h config.def.h config.mk $(SRC) tpl.1 LICENSE-suckless util.h tpl-$(VERSION)
	tar -cf tpl-$(VERSION).tar tpl-$(VERSION)
	gzip tpl-$(VERSION).tar
	rm -rf tpl-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f tpl $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/tpl
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f tpl.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/tpl.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tpl $(DESTDIR)$(MANPREFIX)/man1/tpl.1

.PHONY: all options clean dist install uninstall
