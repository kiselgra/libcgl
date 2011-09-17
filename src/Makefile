CC=gcc -Wall -ggdb3 -O0 -std=gnu99

all: lib blub
.PHONY: clean lib

LIBOBJS=mesh.o shader.o drawelement.o

blub: $(OBJS) lib
	$(CC) blub.c -o $@ libcgl.a -lGLEW -lGL -lGLU -lglut -lmcm

lib: libcgl.a

libcgl.a: $(LIBOBJS)
	ar rcs libcgl.a $(LIBOBJS)


clean:
	rm $(LIBOBJS) blub libcgl.a
