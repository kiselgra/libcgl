CC=gcc -Wall -ggdb3 -O0 -std=gnu99

all: lib blub bspline
.PHONY: clean lib

LIBOBJS=mesh.o shader.o drawelement.o glut.o camera.o cgl.o

blub: blub.c lib
	$(CC) blub.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm

bspline: bspline.c lib
	$(CC) bspline.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm

lib: libcgl.a

libcgl.a: $(LIBOBJS)
	ar rcs libcgl.a $(LIBOBJS)


clean:
	rm $(LIBOBJS) blub libcgl.a
