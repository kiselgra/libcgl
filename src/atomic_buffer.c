#include "atomic_buffer.h"

#include "cgl.h"
#include "gl-version.h"

#include <stdbool.h>
#include <stdlib.h>

// in contrast to "framebuffer", "atomic buffer" uses a blank. maybe this could
// become a more general "buffer" abstraction, soon?

struct atomic_buffer {
	char *name;
	unsigned int width, height;
	GLuint gl_buffer;
	bool mapped;
};

#define TYPE atomic_buffer
#define ARRAY atomic_buffers
#define REF atomic_buffer_ref

#include "mm.h"

atomic_buffer_ref make_atomic_buffer(const char *name, unsigned int w, unsigned int h) {
	atomic_buffer_ref ref = allocate_ref();
	struct atomic_buffer *buf = atomic_buffers + ref.id;

	glGenBuffers(1, &buf->gl_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf->gl_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	buf->mapped = false;
}

void bind_atomic_buffer(atomic_buffer_ref ref, unsigned int location) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, location, buf->gl_buffer);
}

void unbind_atomic_buffer(atomic_buffer_ref ref, unsigned int location) {
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, location, 0);
}

int* map_atomic_int_buffer(atomic_buffer_ref ref, GLenum how) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf->gl_buffer);
	buf->mapped = true;
	return (int*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, how);
}

unsigned int* map_atomic_unsigned_int_buffer(atomic_buffer_ref ref, GLenum how) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf->gl_buffer);
	buf->mapped = true;
	return (unsigned int*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, how);
}

void unmap_atomic_buffer(atomic_buffer_ref ref) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	buf->mapped = false;
}

void reset_atomic_buffer(atomic_buffer_ref ref, int value) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	int *b = map_atomic_int_buffer(ref, GL_WRITE_ONLY);
	for (unsigned int i = 0; i < buf->width * buf->height; ++i)
		b[i] = value;
	unmap_atomic_buffer(ref);
}

void reset_atomic_bufferu(atomic_buffer_ref ref, unsigned int value) {
	struct atomic_buffer *buf = atomic_buffers + ref.id;
	unsigned int *b = map_atomic_int_buffer(ref, GL_WRITE_ONLY);
	for (unsigned int i = 0; i < buf->width * buf->height; ++i)
		b[i] = value;
	unmap_atomic_buffer(ref);
}



/*
GLuint ac_buffer = 0;
glGenBuffers(1, &ac_buffer);
glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);



Now how to enable the atomic counter buffer object:

glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buffer);



And to end up, how to reset the value of the atomic counter:

glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
		sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT |
		GL_MAP_UNSYNCHRONIZED_BIT);
ptr[0] = value;
glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); 
*/

