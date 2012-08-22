BUILT_SOURCES = gl.h.scm

gl.h.scm: /usr/include/GL/gl.h
	echo ";;; generated from the local gl.h by cgl on $$(date)." > $@
	egrep -x '#define\s+GL_\w*\s+\w*' $< >> $@
	sed -ie 's/#define\s*GL_\(\w*\)\s\+\(\w*\)/(define gl:\L\1 \2)/' $@
	sed -ie 's/_/-/g' $@
	sed -ie 's/0x/#x/' $@

scheme_DATA = reader-extensions.scm shader.scm vecnf.scm gl.h.scm
schemedir = $(pkgdatadir)/scheme
EXTRA_DIST = $(scheme_DATA)
