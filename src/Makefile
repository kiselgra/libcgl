CC=gcc -Wall -ggdb3 -O0 -std=gnu99

OBJS=blub.o mesh.o shader.o

blub: $(OBJS)
	$(CC) $(OBJS) -o $@ -lGLEW -lGL -lGLU -lglut -lmcm

.PHONY: clean

clean:
	rm $(OBJS) blub
