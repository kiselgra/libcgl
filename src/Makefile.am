lib_LTLIBRARIES = libcgl.la
noinst_PROGRAMS = #bezier #blub bspline


BUILT_SOURCES = atomic_buffer.xx shader.xx camera.xx

if CAN_USE_GUILE
BUILT_SOURCES+=camera.x glut.x shader.x texture.x framebuffer.x mesh.x impex.x prepared.x scheme.x atomic_buffer.x gl.wrappers.x
endif

# libconfigfiles_la_SOURCES = grammar.yy  lexer.ll  parser.h  token.h lexer.h config_creator.h configfile.h configfile.cpp
libcgl_la_SOURCES = 	atomic_buffer.c atomic_buffer.h \
						camera.c       camera.h  \
                        cgl.c          cgl.h          \
                        drawelement.c  drawelement.h  \
                        framebuffer.c  framebuffer.h  \
                        glut.c         glut.h     \
						gl-version.h \
                        impex.cpp      impex.h      \
						libcgl.h \
                        mesh.c         mesh.h  \
						objloader.cpp  objloader.h \
						scheme.c       scheme.h \
                        shader.c       shader.h  \
                        texture.c      texture.h \
						mm.h \
						prepared.cpp   prepared.h \
						gl.wrappers.c \
						cgl-config.h


#blub_SOURCES = $(libcgl_la_SOURCES) blub.c
bezier_SOURCES = $(libcgl_la_SOURCES) bezier.c
#bspline_SOURCES = $(libcgl_la_SOURCES) bspline.c

SUFFIXES: .x .xx

if CAN_USE_GUILE
snarf_opts = @GUILE_DEF@ $(AM_CPPFLAGS) $(libcgl_la_CPPFLAGS) $(CPPFLAGS) -D CGL_GL_VERSION=$(GL_VERSION)
.c.x:
	guile-snarf -o $@ $*.c $(snarf_opts)
.cpp.x:
	guile-snarf -o $@ $*.cpp $(snarf_opts)
endif

.c.xx: $< mm.m4
	m4 mm.m4 $*.c > $@

# for bison/flex
# AM_CPPFLAGS = -Isrc/
# AM_YFLAGS = -d -p__yacc_prefix__
# AM_LFLAGS = -P__lex_prefix__ -o lex.yy.c
CGL_AM_CPPFLAGS = $(GUILE_CFLAGS) @GUILE_DEF@ -D CGL_GL_VERSION=$(GL_VERSION) $(GLEW_CFLAGS)

library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
library_include_HEADERS = atomic_buffer.h cgl.h camera.h drawelement.h framebuffer.h glut.h gl-version.h impex.h libcgl.h mesh.h shader.h texture.h objloader.h scheme.h mm.h cgl-config.h prepared.h

# setting c++ flags:
# AM_CXXFLAGS = -Wall -ansi -pedantic -ggdb -pg

# setting c preprocessor flags for main project
libcgl_la_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(GUILE_CFLAGS) -D DATADIR=\"$(pkgdatadir)\" @GUILE_DEF@ $(AM_CPPFLAGS) -D CGL_GL_VERSION=$(GL_VERSION)

libcgl_la_LIBADD = $(GUILE_LDFLAGS) $(OBJL_LDFLAGS) $(GLEW_LIBS)

#blub_CPPFLAGS = $(libcgl_la_CPPFLAGS)
bezier_CPPFLAGS = $(libcgl_la_CPPFLAGS)
#bspline_CPPFLAGS = $(libcgl_la_CPPFLAGS)

#blub_LDADD = $(GUILE_LDFLAGS)
bezier_LDADD = $(GUILE_LDFLAGS) $(GLEW_LIBS)
#bspline_LDADD = $(GUILE_LDFLAGS)

if CAN_USE_GUILE
scheme.c camera.c shader.c texture.c glut.c mesh.c impex.cpp: shader.x camera.x texture.x glut.x mesh.x impex.x
gl.wrappers.c: gl.wrappers.x
endif
camera.c: camera.xx

# install link
library_nonversioned_includedir = $(includedir)/$(PACKAGE)
install-data-hook:
	@test -L $(library_nonversioned_includedir) && echo "   !!!   The nonversioned library include dir <$(library_nonversioned_includedir)> is already covered by a link. You'll have to update yourself". || true
	test -L $(library_nonversioned_includedir) || ln -s $(library_includedir) $(library_nonversioned_includedir)

libcgldir = $(pkgdatadir)
libcgl_DATA = mm.m4
EXTRA_DIST = mm.m4
