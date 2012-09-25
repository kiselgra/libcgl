#include "atomic_buffer.h"

#include "cgl.h"
#include "gl-version.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// in contrast to "framebuffer", "atomic buffer" uses a blank. maybe this could
// become a more general "buffer" abstraction, soon?

struct atomic_buffer {
	char *name;
	unsigned int width, height;
	GLuint gl_buffer;
	bool mapped;
};

// #include "mm.h"

#define define_mm(X,Y,Z)
define_mm(atomic_buffer, atomic_buffers, atomic_buffer_ref)
#include "atomic_buffer.xx"

atomic_buffer_ref make_atomic_buffer(const char *name, unsigned int w, unsigned int h) {
	atomic_buffer_ref ref = allocate_atomic_buffer_ref();
	struct atomic_buffer *buf = atomic_buffers + ref.id;

	glGenBuffers(1, &buf->gl_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf->gl_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, w*h*sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	buf->width = w;
	buf->height = h;
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

atomic_buffer_ref find_atomic_buffer(const char *name) {
	atomic_buffer_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_atomic_buffer_index; ++i)
		if (strcmp(atomic_buffers[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	return ref;
}

char* atomic_buffer_name(atomic_buffer_ref ref) {
	struct atomic_buffer *atomic_buffer = atomic_buffers+ref.id;
    return atomic_buffer->name;
}



#ifdef WITH_GUILE
#include  <libguile.h>
#include <stdio.h>

SCM_DEFINE(s_make_atomic_buffer, "make-atomic-buffer", 3, 0, 0, (SCM name, SCM w, SCM h), "") {
	char *n = scm_to_locale_string(name);
	atomic_buffer_ref ref = make_atomic_buffer(n, scm_to_int(w), scm_to_int(h));
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_bind_atomic_buffer, "bind-atomic-buffer", 2, 0, 0, (SCM id, SCM to), "") {
	atomic_buffer_ref ref = { scm_to_int(id) };
	bind_atomic_buffer(ref, scm_to_int(to));
	return SCM_BOOL_T;
}

SCM_DEFINE(s_unbind_atomic_buffer, "unbind-atomic-buffer", 2, 0, 0, (SCM id, SCM to), "") {
	atomic_buffer_ref ref = { scm_to_int(id) };
	unbind_atomic_buffer(ref, scm_to_int(to));
	return SCM_BOOL_T;
}

SCM_DEFINE(s_reset_atomic_buffer, "reset-atomic-buffer", 2, 0, 0, (SCM id, SCM to), "") {
	atomic_buffer_ref ref = { scm_to_int(id) };
	reset_atomic_buffer(ref, scm_to_int(to));
	return SCM_BOOL_T;
}

SCM_DEFINE(s_find_atomic_buffer, "find-atomic-buffer", 1, 0, 0, (SCM name), "") {
	char *n = scm_to_locale_string(name);
	atomic_buffer_ref ref = find_atomic_buffer(n);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_atomic_buffer_name, "atomic-buffer-name", 1, 0, 0, (SCM ab), "") {
	atomic_buffer_ref ref = { scm_to_int(ab) };
	return scm_from_locale_string(atomic_buffer_name(ref));
}

SCM_DEFINE(s_map_atomic_buffer, "read-atomic-buffer", 1, 0, 0, (SCM id), "") {
	atomic_buffer_ref ref = { scm_to_int(id) };
	int *data = map_atomic_int_buffer(ref, GL_READ_ONLY);
	struct atomic_buffer *buf = atomic_buffers+ref.id;
	int bytes = buf->width * buf->height * sizeof(int);
	SCM bv = scm_c_make_bytevector(bytes);
	memcpy(SCM_BYTEVECTOR_CONTENTS(bv), data, bytes);
	unmap_atomic_buffer(ref);
	return bv;
}

void register_scheme_functions_for_atomic_buffers(void) {
#ifndef SCM_MAGIC_SNARFER
#include "atomic_buffer.x"
#endif
}

#endif


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

