AUTOMAKE_OPTIONS = 1.6 foreign dist-bzip2
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2

SUBDIRS =
DIST_SUBDIRS = doc
VERSION_INFO = @VERSION_INFO@
RELEASE_INFO = @RELEASE_INFO@

lib_LTLIBRARIES = libSDL_stretch.la 
pkglib_LTLIBRARIES = libSDL_stretch_test.la
pkglib_PROGRAMS = check1 parallax2

libSDL_stretch_la_SOURCES = sdlstretch.c sdlstretch23.c sdlstretchcode.c
libSDL_stretch_la_LDFLAGS = $(RELEASE_INFO) $(VERSION_INFO)
pkginclude_HEADERS = SDL_stretch.h SDL_stretchcode.h SDL_stretchasm.h

libSDL_stretch_test_la_SOURCES = sdlscreen.c 
libSDL_stretch_test_la_LDFLAGS = $(RELEASE_INFO) $(VERSION_INFO)
libSDL_stretch_test_la_LIBADD = libSDL_stretch.la
pkglib_HEADERS = SDL_stretchtest.hh

check1_LDADD = libSDL_stretch.la
parallax2_LDADD = libSDL_stretch.la

pkgconfiglibdir = ${libdir}/pkgconfig
pkgconfiglib_DATA = $(PACKAGE).pc

EXTRA_DIST = sdlscreen.c $(PACKAGE).spec \
             stretch.txt parallax2.bmp

rpm : dist-bzip2 $(PACKAGE).spec
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2

upload www docs install-docs install-man3 \
manpages.ar htmpages.ar doc/manpages.ar doc/htmpages.ar :
	(cd doc && $(MAKE) `basename $@`)
