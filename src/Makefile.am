lib_LTLIBRARIES = libcgl.la
noinst_PROGRAMS = bezier blub bspline

# libconfigfiles_la_SOURCES = grammar.yy  lexer.ll  parser.h  token.h lexer.h config_creator.h configfile.h configfile.cpp
libcgl_la_SOURCES = 	camera.c       camera.h  \
                        cgl.c          cgl.h          \
                        drawelement.c  drawelement.h  \
                        framebuffer.c  framebuffer.h  \
                        glut.c         glut.h     \
                        impex.cpp      impex.h      \
                        mesh.c         mesh.h  \
						scheme.c       scheme.h \
                        shader.c       shader.h  \
                        texture.c      texture.h


blub_SOURCES = $(libcgl_la_SOURCES) blub.c
bezier_SOURCES = $(libcgl_la_SOURCES) bezier.c
bspline_SOURCES = $(libcgl_la_SOURCES) bspline.c


# for bison/flex
# AM_CPPFLAGS = -Isrc/
# AM_YFLAGS = -d -p__yacc_prefix__
# AM_LFLAGS = -P__lex_prefix__ -o lex.yy.c
AM_CPPFLAGS = $(GUILE_CFLAGS)

library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
library_include_HEADERS = cgl.h camera.h drawelement.h framebuffer.h glut.h impex.h mesh.h shader.h texture.h

# setting c++ flags:
# AM_CXXFLAGS = -Wall -ansi -pedantic -ggdb -pg

# setting c preprocessor flags for main project
libcgl_la_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(GUILE_CFLAGS)
libcgl_la_LIBADD = $(GUILE_LIBS)

blub_LDADD = $(GUILE_LIBS)
bezier_LDADD = $(GUILE_LIBS)
bspline_LDADD = $(GUILE_LIBS)
