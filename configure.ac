AC_INIT(SDL_stretch.spec)
AC_PREREQ(2.49)
AC_COPYRIGHT([Guido U. Draheim <guido@gmx.de> for SDL-stretch.SF.net])
AC_REVISION($Revision: 0.0$)
AC_CONFIG_AUX_DIR(use)
AC_CANONICAL_SYSTEM
AC_SET_DEFAULT_PATHS_SYSTEM
AX_SPEC_DEFAULTS
AM_INIT_AUTOMAKE($PACKAGE,$VERSION)
test ".$CFLAGS" = "." && CFLAGS=""
AC_PROG_CC
test ".$CFLAGS" = "." && test "$GCC" = "yes" && CFLAGS="-O3"
AC_PATH_PROG(PYTHON, python ,:)
AC_PATH_PROG(XMLTO,xmlto,:)
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AX_SET_VERSION_INFO
AM_CONFIG_HEADER(src/config.h)
dnl AX_PREFIX_CONFIG_H([SDL_stretchconf.h])
AM_MAINTAINER_MODE
AX_CREATE_PKGCONFIG_INFO([src/${PACKAGE}.pc],[\\\\@%:@ SDL])
AM_PATH_SDL(1.0.0,[
LIBS="$LIBS $SDL_LIBS"
CFLAGS="$CFLAGS $SDL_CFLAGS"])
AX_MAINTAINER_MODE_AUTO_SILENT
AX_NOT_ENABLE_FRAME_POINTER
AX_CFLAGS_WARN_ALL
AX_CFLAGS_NO_WRITABLE_STRINGS
AX_CFLAGS_STRICT_PROTOTYPES
AX_CFLAGS_GCC_OPTION(,-Wmissing-declarations,)
AX_WARNING_DEFAULT_PKGCONFIGDIR
AC_OUTPUT([Makefile src/Makefile doc/Makefile win32/Makefile])
PATCH_LIBTOOL_ON_DARWIN_ZSH_OVERQUOTING
PATCH_LIBTOOL_SYS_LIB_SEARCH_PATH_SPEC
PATCH_LIBTOOL_TO_ADD_HOST_CC
echo '-----' $PACKAGE $VERSION $prefix
$SHELL libtool --features || true
echo "CFLAGS=$CFLAGS"
