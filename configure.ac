#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([libcgl], [0.0.1], [((lambda (x y z) (string-append x "@" y "." z ".org")) "stuff" "projects" "selgrad")])
AM_INIT_AUTOMAKE

# name one of your source files here
AC_CONFIG_SRCDIR([src/mesh.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile src/Makefile libcgl.pc)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99

GUILE_FLAGS

# PKG_CHECK_MODULES([GUILE], [guile-2.0])
# AC_SUBST([GUILE_CFLAGS])
# AC_SUBST([GUILE_LIBS])

# when using flex/bison:
# AC_PROG_LEX
# AC_PROG_YACC

# when building libraries
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([mcm], [make_vec3f])
AC_CHECK_LIB([png], [main])

AC_ARG_WITH([gl], [AS_HELP_STRING([--with-gl=X], [Choose which gl version you want. Options: gl, gles (meaning gles2), use lowercase.])], glver=$withval, glver="gl")
AS_IF([test "x$glver" == "xgl"],
	[
		AC_CHECK_LIB([GL], [main])
		AC_CHECK_LIB([GLU], [main])
		AC_CHECK_LIB([glut], [main])
		AC_CHECK_LIB([GLEW], [main])
		AC_SUBST([GL_VERSION], [GL3])
	],
	[test "x$glver" == "xgles"],
	[
		AC_CHECK_LIB([X11], [main])
		AC_CHECK_LIB([eglut_x11], [main])
		AC_CHECK_LIB([EGL], [main])
		AC_CHECK_LIB([GLESv2], [main])
		AC_SUBST([GL_VERSION], [GLES2])
	],
	[
		AC_MSG_ERROR([Invalid gl version. Use (literally) gl or gles.])
	])

AC_CHECK_LIB([objloader], [main])
AC_CHECK_LIB([3dmath], [main])
#AC_PATH_PROG([OBJL_CONFIG], [libobjloader-config])
#if test "x$ac_cv_path_OBJL_CONFIG" = "x"; then
#	AC_MSG_ERROR([libobjloader not found. get it somewhere at server1.darkcellarnights.de/~kai/projekte])
#fi
#OBJL_CPPFLAGS=`$OBJL_CONFIG -c`
#OBJL_LDFLAGS=`$OBJL_CONFIG -l`
#AC_SUBST([OBJL_CPPFLAGS])
#AC_SUBST([OBJL_LDFLAGS])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
