#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([libcgl], [0.0.1])
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

# name one of your source files here
AC_CONFIG_SRCDIR([src/mesh.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile wrapper-gen/Makefile src/Makefile scheme/Makefile libcgl.pc)

AX_PREFIX_CONFIG_H(src/cgl-config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O

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

AC_ARG_WITH([guile], [AS_HELP_STRING([--with-guile], [Wether to use guile. Default: yes.])], choose_guile=$withval, choose_guile="yes")
AS_IF(
	[test "x$choose_guile" == "xyes"],
	[
		ifdef([GUILE_FLAGS], [can_use_guile="yes"], [can_use_guile="no"])
		if test "x$can_use_guile" = "xyes" ; then
			GUILE_FLAGS
			GUILE_DEF="-DWITH_GUILE"
		else
			GUILE_DEF=""
		fi
	],
	[
		GUILE_DEF=""
	])
AC_SUBST([GUILE_DEF])
AM_CONDITIONAL([CAN_USE_GUILE], [test "x$can_use_guile" = "xyes"])

AC_ARG_WITH([gl], [AS_HELP_STRING([--with-gl=X], [Choose which gl version you want. Options: gl3, gl4, gl (meaning gl4), gles (meaning gles2), use lowercase.])], glver=$withval, glver="gl")
AS_IF(
	[test "x$glver" == "xgl3"],
	[
		AC_CHECK_LIB([GL], [main])
		AC_CHECK_LIB([GLU], [main])
		AC_CHECK_LIB([glut], [main])

		PKG_CHECK_MODULES(GLEW, glew>=1.8)
		AC_SUBST(GLEW_CFLAGS)
		AC_SUBST(GLEW_LIBS)

		AC_SUBST([GL_VERSION], [GL3])
	],
	[test "x$glver" == "xgl4" || test "x$glver" == "xgl" ],
	[
		AC_CHECK_LIB([GL], [main])
		AC_CHECK_LIB([GLU], [main])
		AC_CHECK_LIB([glut], [main])

		PKG_CHECK_MODULES(GLEW, glew)
		AC_SUBST(GLEW_CFLAGS)
		AC_SUBST(GLEW_LIBS)

		AC_SUBST([GL_VERSION], [GL4])
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
		AC_MSG_ERROR([Invalid gl version. Use (literally) gl, gl3, gl4 or gles.])
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

AC_ARG_WITH([jpeg],
            [AS_HELP_STRING([--with-jpeg],
                            [Support loading of jpeg files through libjpeg. Does not set any paths. @<:@default=check@:>@])],
		    [with_jpeg=$withval],
			[with_jpeg=check])
AS_CASE(["$with_jpeg"],
		[check], [AC_CHECK_LIB([jpeg], [main])],
		[no], [],
		[AC_DEFINE([HAVE_LIBPNG], [1], [Via --with-jpeg=PATH])])

AC_ARG_WITH([magickwand],
			[AS_HELP_STRING([--with-magickwand],
							[Support loading images files using the MagickWand library from imagemagick.])],
			[with_magickwand=$withval],
			[with_magickwand=check])
AS_CASE(["$with_magickwand"],
		[check], [	PKG_CHECK_MODULES(WAND, MagickWand,
									  [HAVE_MAGICKWAND=1],
									  [HAVE_MAGICKWAND=0])
					AC_SUBST(WAND_CFLAGS)
					AC_SUBST(WAND_LIBS)
					AC_DEFINE_UNQUOTED([HAVE_MAGICKWAND], [$HAVE_MAGICKWAND], [Set to 1 if you have libMagicWand of imagemagick.])
					],
		[no], [],
		[AC_MSG_ERROR([dont know how to do this])])

AC_CHECK_LIB([3dmath], [main])
#AC_PATH_PROG([OBJL_CONFIG], [libobjloader-config])
#if test "x$ac_cv_path_OBJL_CONFIG" = "x"; then
#	AC_MSG_ERROR([libobjloader not found. get it somewhere at server1.darkcellarnights.de/~kai/projekte])
#fi
#OBJL_CPPFLAGS=`$OBJL_CONFIG -c`
#OBJL_LDFLAGS=`$OBJL_CONFIG -l`
#AC_SUBST([OBJL_CPPFLAGS])
#AC_SUBST([OBJL_LDFLAGS])

AC_ARG_ENABLE(gdb,
			[AS_HELP_STRING([--enable-gdb], [Wether to use debugging flags. Default: no.])],
			with_debug_flags="$enableval", with_debug_flags="no")
if test "x$with_debug_flags" == "xyes" ; then
	DEBUG_FLAGS="-ggdb3 -O0"
else
	DEBUG_FLAGS=""
fi
CFLAGS="$CFLAGS $DEBUG_FLAGS"
CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS"


AC_LINK_IFELSE(
	[AC_LANG_CALL([],[glProgramParameteri])],
	[],
	[AC_DEFINE(
		[SKIP_SCM_glProgramParameteri],,
		[Dont export this function to scheme as it will generate linker errors in Ubuntu 12.04´s Nvidia driver])],
	)

AC_LINK_IFELSE(
	[AC_LANG_CALL([],[glUniformBlockBinding])],
	[],
	[AC_DEFINE(
		[SKIP_SCM_glUniformBlockBinding],,
		[Dont export this function to scheme as it will generate linker errors in Ubuntu 12.04´s Nvidia driver])],
	)

#AC_LANG_CONFTEST(
#	[AC_LANG_PROGRAM([glUniform3f();],[glUniform3f();])])
#AC_LANG_PROGRAM([],[glProgramParameteri])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
