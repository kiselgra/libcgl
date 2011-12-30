lib_LTLIBRARIES = libcgl.la
#noinst_PROGRAMS = bezier blub bspline

BUILT_SOURCES=camera.x glut.x shader.x texture.x framebuffer.x mesh.x

# libconfigfiles_la_SOURCES = grammar.yy  lexer.ll  parser.h  token.h lexer.h config_creator.h configfile.h configfile.cpp
libcgl_la_SOURCES = 	camera.c       camera.h  \
                        cgl.c          cgl.h          \
                        drawelement.c  drawelement.h  \
                        framebuffer.c  framebuffer.h  \
                        glut.c         glut.h     \
						gl-version.h \
                        impex.cpp      impex.h      \
                        mesh.c         mesh.h  \
						objloader.cpp  objloader.h \
						scheme.c       scheme.h \
                        shader.c       shader.h  \
                        texture.c      texture.h \
						mm.h


blub_SOURCES = $(libcgl_la_SOURCES) blub.c
bezier_SOURCES = $(libcgl_la_SOURCES) bezier.c
bspline_SOURCES = $(libcgl_la_SOURCES) bspline.c


snarf_opts = -D WITH_GUILE $(AM_CPPFLAGS) $(libcgl_la_CPPFLAGS) $(CPPFLAGS) -D CGL_GL_VERSION=$(GL_VERSION)
SUFFIXES: .x
.c.x:
	guile-snarf -o $@ $< $(snarf_opts)

# for bison/flex
# AM_CPPFLAGS = -Isrc/
# AM_YFLAGS = -d -p__yacc_prefix__
# AM_LFLAGS = -P__lex_prefix__ -o lex.yy.c
CGL_AM_CPPFLAGS = $(GUILE_CFLAGS) -D WITH_GUILE -D CGL_GL_VERSION=$(GL_VERSION)

library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
library_include_HEADERS = cgl.h camera.h drawelement.h framebuffer.h glut.h gl-version.h impex.h mesh.h shader.h texture.h objloader.h scheme.h mm.h

# setting c++ flags:
# AM_CXXFLAGS = -Wall -ansi -pedantic -ggdb -pg

# setting c preprocessor flags for main project
libcgl_la_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(GUILE_CFLAGS) -D WITH_GUILE $(AM_CPPFLAGS) -D CGL_GL_VERSION=$(GL_VERSION)

libcgl_la_LIBADD = $(GUILE_LDFLAGS) $(OBJL_LDFLAGS)

blub_CPPFLAGS = $(libcgl_la_CPPFLAGS)
bezier_CPPFLAGS = $(libcgl_la_CPPFLAGS)
bspline_CPPFLAGS = $(libcgl_la_CPPFLAGS)

blub_LDADD = $(GUILE_LDFLAGS)
bezier_LDADD = $(GUILE_LDFLAGS)
bspline_LDADD = $(GUILE_LDFLAGS)

scheme.c camera.c shader.c texture.c glut.c mesh.c: shader.x camera.x texture.x glut.x mesh.x

