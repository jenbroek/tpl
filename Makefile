# tpl - shell templating utility
# See LICENSE file for copyright and license details.

include config.mk

SRC = tpl.c util.c
OBJ = ${SRC:.c=.o}

all: options tpl

options:
	@echo tpl build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk util.h

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

tpl: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f tpl ${OBJ} tpl-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p tpl-${VERSION}
	@cp -R LICENSE Makefile README.md config.def.h config.mk ${SRC} LICENSE-suckless util.h tpl-${VERSION}
	@tar -cf tpl-${VERSION}.tar tpl-${VERSION}
	@gzip tpl-${VERSION}.tar
	@rm -rf tpl-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f tpl ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/tpl

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/tpl

.PHONY: all options clean dist install uninstall
