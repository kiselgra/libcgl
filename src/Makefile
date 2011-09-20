CC=gcc -Wall -ggdb3 -O0 -std=gnu99
#CPPFLAGS=
LDFLAGS=-lpng

all: lib blub bspline bezier
.PHONY: clean lib

LIBOBJS=mesh.o shader.o drawelement.o glut.o camera.o cgl.o texture.o impex.o framebuffer.o

blub: blub.c lib
	$(CC) blub.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm $(LDFLAGS)

bspline: bspline.c lib
	$(CC) bspline.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm

bezier: bezier.c lib
	$(CC) bezier.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm

lib: libcgl.a

libcgl.a: $(LIBOBJS)
	ar rcs libcgl.a $(LIBOBJS)


clean:
	rm $(LIBOBJS) blub libcgl.a
