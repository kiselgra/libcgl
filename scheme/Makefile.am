BUILT_SOURCES = gl.h.scm

gl.h.scm: $(top_builddir)/wrapper-gen/gl3.h Makefile
	echo ";;; generated from the distributed gl3.h by cgl on $$(date)." > $@
	egrep -x '#define\s+GL_\w*\s+\w*' $< >> $@
	sed -ie 's/#define\s\+\(.*\)/(define \1)/' $@
	sed -ie 's/GL_\(\w*\)/gl#\L\1/g' $@
	sed -ie 's/_/-/g' $@
	sed -ie 's/0x/#x/' $@
	sed -ie 's/\(#x.*\)u.*)/\L\1)/' $@

scheme_DATA = reader-extensions.scm shader.scm vecnf.scm gl.h.scm cgl-init.scm
schemedir = $(pkgdatadir)/scheme
EXTRA_DIST = $(scheme_DATA)
