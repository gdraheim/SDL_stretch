AUTOMAKE_OPTIONS = 1.4 foreign dist-bzip2
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2

SUBDIRS =
DIST_SUBDIRS = doc

lib_LTLIBRARIES = libSDL_stretch.la

libSDL_stretch_la_SOURCES = sdlstretch.c sdlstretch23.c sdlstretchcode.c
pkginclude_HEADERS = SDL_stretch.h SDL_stretchcode.h

pkgconfiglibdir = ${libdir}/pkgconfig
pkgconfiglib_DATA = $(PACKAGE).pc

EXTRA_DIST = sdlscreen.c $(PACKAGE).spec

rpm : dist-bzip2 $(PACKAGE).spec
	rpm -ta $(PACKAGE)-$(VERSION).tar.bz2

docs install-docs install-man3 \
manpages.ar htmpages.ar doc/manpages.ar doc/htmpages.ar :
	(cd doc && $(MAKE) `basename $@`)
