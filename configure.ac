#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([libcgl], [0.0.1])
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

# name one of your source files here
AC_CONFIG_SRCDIR([src/mesh.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile src/Makefile scheme/Makefile libcgl.pc)

AX_PREFIX_CONFIG_H(src/cgl-config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99

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
#AC_CHECK_LIB([png], [main])

ifdef([GUILE_FLAGS], [can_use_guile="yes"], [can_use_guile="no"])
if test "x$can_use_guile" = "xyes" ; then
	GUILE_FLAGS
	GUILE_DEF="-DWITH_GUILE"
else
	GUILE_DEF=""
fi
AC_SUBST([GUILE_DEF])
AM_CONDITIONAL([CAN_USE_GUILE], [test "x$can_use_guile" = "xyes"])

AC_ARG_WITH([gl], [AS_HELP_STRING([--with-gl=X], [Choose which gl version you want. Options: gl, gles (meaning gles2), use lowercase.])], glver=$withval, glver="gl")
AS_IF([test "x$glver" == "xgl"],
	[
		AC_CHECK_LIB([GL], [main])
		AC_CHECK_LIB([GLU], [main])
		AC_CHECK_LIB([glut], [main])

#		AC_CHECK_LIB([GLEW], [main])
		PKG_CHECK_MODULES(GLEW, glew)
		AC_SUBST(GLEW_CFLAGS)
		AC_SUBST(GLEW_LIBS)

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

AC_ARG_WITH([objloader],
            [AS_HELP_STRING([--with-objloader],
                            [Support loading of wavefront obj files. Does not set any paths. @<:@default=check@:>@])],
		    [with_objloader=$withval],
			[with_objloader=check])
AS_CASE(["$with_objloader"],
		[check], [AC_CHECK_LIB([objloader], [main])],
		[no], [],
		[AC_DEFINE([HAVE_LIBOBJLOADER], [1], [Via --with-objloader=PATH])])


AC_ARG_WITH([png],
            [AS_HELP_STRING([--with-png],
                            [Support loading of png files through libpng and png++. Does not set any paths. @<:@default=check@:>@])],
		    [with_png=$withval],
			[with_png=check])
AS_CASE(["$with_png"],
		[check], [AC_CHECK_LIB([png], [main])],
		[no], [],
		[AC_DEFINE([HAVE_LIBPNG], [1], [Via --with-png=PATH])])



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
