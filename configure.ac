#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ming], [0.3.0], [gkmail@sina.com])
AC_CONFIG_SRCDIR([include/m_arch.h])
AC_CONFIG_HEADERS([include/m_config.h])

# Detect the canonical host and target build environment.
AC_CANONICAL_SYSTEM

# Automake initialize.
AM_INIT_AUTOMAKE

# Package version.
# Making releases:
#   MICRO_VERSION += 1;
#   INTERFACE_AGE += 1;
#   BINARY_AGE += 1;
# if any functions have been added, set INTERFACE_AGE to 0.
# if backwards compatibility has been broken, set BINARY_AGE and INTERFACE_AGE to 0.

MAJOR_VERSION=0
MINOR_VERSION=3
MICRO_VERSION=0
INTERFACE_AGE=0
BINARY_AGE=0
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_SUBST(INTERFACE_AGE)
AC_SUBST(BINARY_AGE)
AC_SUBST(VERSION)

#Libtool versioning.
LT_RELEASE=$MAJOR_VERSION.$MINOR_VERSION
LT_CURRENT=`expr $MICRO_VERSION - $INTERFACE_AGE`
LT_REVISION=$INTERFACE_AGE
LT_AGE=`expr $BINARY_AGE - $INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_CONFIG_MACRO_DIR([m4])
PKG_PROG_PKG_CONFIG()
AC_PROG_LIBTOOL
IT_PROG_INTLTOOL([0.35])
AC_LIBTOOL_WIN32_DLL

# Gettext definition.
ALL_LINGUAS="zh_CN"
GETTEXT_PACKAGE="$PACKAGE-$MAJOR_VERSION.$MINOR_VERSION"

CFLAGS="-g -O2"
LIBBS=

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h limits.h stdlib.h string.h unistd.h])

# Check for <assert.h>
AC_HEADER_ASSERT

# Check for <stdio.h>.
AC_CHECK_HEADER([stdio.h],
				AC_DEFINE_UNQUOTED([HAVE_STDIO_H], 1, [Define to 1 if you have the <stdio.h> header file.]))

# Check for <ctype.h>.
AC_CHECK_HEADER([ctype.h],
				AC_DEFINE_UNQUOTED([HAVE_CTYPE_H], 1, [Define to 1 if you have the <ctype.h> header file.]))

# Check for pthread header file and lbrary.
AC_CHECK_HEADER([pthread.h],
				AC_DEFINE_UNQUOTED([HAVE_PTHREAD_H], 1, [Define to 1 if you have the <pthread.h> header file.]))
AC_SEARCH_LIBS([pthread_create], [pthread],
		[LIBS="$LIBS -lpthread"], AC_MSG_ERROR(Cannot find "pthread.h"))

# Check for math header and library.
AC_CHECK_HEADER([math.h],
				AC_DEFINE_UNQUOTED([HAVE_MATH_H], 1, [Define to 1 if you have the <math.h> header file.]))
AC_SEARCH_LIBS([ceil], [m],
		[LIBS="$LIBS -lm"], AC_MSG_ERROR(Cannot find "math.h"))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit clock_gettime memset strcasecmp strchr])

AC_OUTPUT([
	Makefile
	include/Makefile
	src/Makefile
	tests/Makefile
	po/Makefile.in
	])
