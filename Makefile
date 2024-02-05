# tpl - shell templating utility
# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

SRC = tpl.c util.c
OBJ = $(SRC:.c=.o)
DEPS = config.h arg.h util.h

all: options tpl

options:
	@echo tpl build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

$(OBJ): $(DEPS) config.mk

config.h:
	cp config.def.h $@

tpl: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f tpl $(OBJ) tpl-$(VERSION).tar.gz

dist: clean
	mkdir -p tpl-$(VERSION)
	cp -R LICENSE LICENSE-suckless README.md config.mk Makefile $(SRC) $(DEPS) tpl.1 tpl-$(VERSION)
	tar czf tpl-$(VERSION).tar.gz tpl-$(VERSION)
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
