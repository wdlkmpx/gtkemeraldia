#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([xemeraldia],[0.4.3],[])
AM_INIT_AUTOMAKE([-Wall foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_GNU_GETTEXT([external])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

LT_INIT

# Init intltool
IT_PROG_INTLTOOL

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

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

case "$CC" in
  gcc*) CPPFLAGS="$CPPFLAGS -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers" ;;
  *) ;;
esac

if [[ "$prefix" = NONE ]]; then
	zzz="$prefix"
	prefix="$ac_default_prefix"
	resolveddatadir=$(eval echo $datadir)
	prefix="$zzz"
else
	resolveddatadir=$(eval echo $datadir)
fi

AC_SUBST(resolveddatadir)

GETTEXT_PACKAGE=xemeraldia
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

AM_GLIB_GNU_GETTEXT


AC_CONFIG_FILES([
	Makefile
	src/Makefile
	po/Makefile.in
	xemeraldia.desktop
])
AC_OUTPUT
