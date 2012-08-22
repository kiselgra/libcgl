#/bin/bash
defines="-D GL_ATI_blend_equation_separate=0"
sed /usr/include/GL/gl.h -e 's:#\s*include:// would include:g' | cpp $defines > work.gl.h
sed -ie 's/__attribute__((visibility("default")))//' work.gl.h
sed -ie 's/typedef .*;//' work.gl.h
sed -ie '/^\s*$/d' work.gl.h
sbcl --noinform --load parse.lisp --eval '(quit)' work.gl.h gl.wrappers.c
guile-snarf -o gl.wrappers.c.x $(pkg-config --cflags guile-2.0) gl.wrappers.c
