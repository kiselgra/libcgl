BUILT_SOURCES = gl.wrappers.c
EXTRA_DIST = gl3.h parse.lisp generate.sh yacc.lisp

gl.wrappers.c: parse.lisp Makefile generate.sh
	sh generate.sh
	cp gl.wrappers.c ../src/
#cp gl.wrappers.c gl.wrappers.c.x ../src/


