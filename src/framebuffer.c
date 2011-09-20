#include "framebuffer.h"

#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct framebuffer {
	char *name;
	unsigned int width, height;
	int max_number_of_color_attachments;
	GLuint fbo_id;
	bool bound;
	GLenum *color_attachments; // gl requires an array, so we might as well save it.
	char **color_attachment_names;
	unsigned int attachments_in_use;
	GLuint depthbuffer;
	char *depthbuffer_name;
};

static struct framebuffer *framebuffers = 0;
static unsigned int framebuffers_allocated = 0, 
                    next_framebuffer_index = 0;

static void allocate_framebuffer() {
	// maintain framebuffer table
	if (next_framebuffer_index >= framebuffers_allocated) {
		struct framebuffer *old_array = framebuffers;
		unsigned int allocate = 1.5 * (framebuffers_allocated + 1);
		framebuffers = malloc(sizeof(struct framebuffer) * allocate);
		for (int i = 0; i < framebuffers_allocated; ++i)
			framebuffers[i] = old_array[i];
		framebuffers_allocated = allocate;
		free(old_array);
	}
}

framebuffer_ref make_framebuffer(char *name, unsigned int width, unsigned int height) {
	allocate_framebuffer();
	framebuffer_ref ref;
	ref.id = next_framebuffer_index++;
	struct framebuffer *framebuffer = framebuffers+ref.id;
	framebuffer->name = malloc(strlen(name)+1);
	strcpy(framebuffer->name, name);

	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &framebuffer->max_number_of_color_attachments);
	framebuffer->color_attachments = malloc(sizeof(GLenum) * framebuffer->max_number_of_color_attachments);
	framebuffer->color_attachment_names = malloc(sizeof(char*) * framebuffer->max_number_of_color_attachments);
	framebuffer->attachments_in_use = 0;
	framebuffer->width = width;
	framebuffer->height = height;
	glGenFramebuffers(1, &framebuffer->fbo_id);
	framebuffer->bound = false;
	framebuffer->depthbuffer = 0;

	return ref;
}

void attach_texture_as_colorbuffer(framebuffer_ref ref, char *name, texture_ref texture) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	if (framebuffer->attachments_in_use == framebuffer->max_number_of_color_attachments) {
		fprintf(stderr, "Cannot attach yet another buffer (%s) (the %d th) to the fbo %s.", name, framebuffer->max_number_of_color_attachments, framebuffer->name);
		exit(-1);
	}
	int id = framebuffer->attachments_in_use++;
	framebuffer->color_attachment_names[id] = malloc(strlen(name)+1);
	strcpy(framebuffer->color_attachment_names[id], name);
	printf("attaching texture id %d to framebuffer %s as att #%d.\n", texture_id(texture), framebuffer->name, id);
	framebuffer->color_attachments[id] = GL_COLOR_ATTACHMENT0 + id;
	glFramebufferTexture2D(GL_FRAMEBUFFER, framebuffer->color_attachments[id], GL_TEXTURE_2D, texture_id(texture), 0);
}

void attach_texture_as_depthbuffer(framebuffer_ref ref, char *name, texture_ref texture) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	framebuffer->depthbuffer_name = malloc(strlen(name)+1);
	strcpy(framebuffer->depthbuffer_name, name);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id(texture), 0);
}


void attach_depth_buffer(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	static const char *default_name = "default depthbuffer";
	framebuffer->depthbuffer_name = malloc(strlen(default_name)+1);
	strcpy(framebuffer->depthbuffer_name, default_name);

	glGenRenderbuffers(1, &framebuffer->depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, framebuffer->width, framebuffer->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthbuffer);
	// unbind renderbuffer?
}

void check_framebuffer_setup(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	if (!framebuffer->depthbuffer) {
		fprintf(stderr, "Framebuffer %s does not contain a depthbuffer.\n", framebuffer->name);
		exit(-1);
	}
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Frammebuffer %s is not complete. Errorcode %d\n", framebuffer->name, status);
		exit(-1);
	}
}

static int old_vp[4];
void bind_framebuffer(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
		
	// well, for now...
	glGetIntegerv(GL_VIEWPORT, old_vp);
	glViewport(0,0, framebuffer->width, framebuffer->height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthbuffer);
	glDrawBuffers(framebuffer->attachments_in_use, framebuffer->color_attachments);
	framebuffer->bound = true;
}

void unbind_framebuffer(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	framebuffer->bound = false;
	glViewport(old_vp[0], old_vp[1], old_vp[2],  old_vp[3]);
}

