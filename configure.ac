#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([xemeraldia],[0.4.3],[nick@reloco.com.ar])
AC_CONFIG_SRCDIR([bitmaps.h])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)
AM_GNU_GETTEXT_VERSION([0.17])

AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h sys/file.h sys/time.h unistd.h fcntl.h pwd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([select flock lockf getopt_long_only setregid])
AC_REPLACE_FUNCS([usleep])

PKG_CHECK_MODULES(XEMERALDIA, gtk+-2.0 >= 2.2)

CFLAGS="$CFLAGS $GTK_CFLAGS"
LIBS="$LIBS $GTK_LIBS"

AM_GNU_GETTEXT([external])

if [[ $GCC = yes ]]; then
	CFLAGS="$CFLAGS -Wall -Wshadow -Wwrite-strings -W -Wstrict-prototypes"
fi

if [[ "$prefix" = NONE ]]; then
	zzz="$prefix"
	prefix="$ac_default_prefix"
	resolveddatadir=$(eval echo $datadir)
	prefix="$zzz"
else
	resolveddatadir=$(eval echo $datadir)
fi

AC_SUBST(resolveddatadir)

AC_CONFIG_FILES([Makefile po/Makefile.in m4/Makefile xemeraldia.desktop])
AC_OUTPUT
