#include "framebuffer.h"

#include "cgl.h"

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
	texture_ref *color_textures;
	char **color_attachment_names;
	unsigned int attachments_in_use;
	GLuint depthbuffer;
	texture_ref depth_tex;
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

framebuffer_ref make_framebuffer(const char *name, unsigned int width, unsigned int height) {
	allocate_framebuffer();
	framebuffer_ref ref;
	ref.id = next_framebuffer_index++;
	struct framebuffer *framebuffer = framebuffers+ref.id;
	framebuffer->name = malloc(strlen(name)+1);
	strcpy(framebuffer->name, name);

#if CGL_GL_VERSION == GL3
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &framebuffer->max_number_of_color_attachments);
#else
	framebuffer->max_number_of_color_attachments = 1;
#endif
	framebuffer->color_attachments = malloc(sizeof(GLenum) * framebuffer->max_number_of_color_attachments);
	framebuffer->color_attachment_names = malloc(sizeof(char*) * framebuffer->max_number_of_color_attachments);
	framebuffer->color_textures = malloc(sizeof(texture_ref) * framebuffer->max_number_of_color_attachments);
	framebuffer->attachments_in_use = 0;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->depth_tex.id = -1;
	glGenFramebuffers(1, &framebuffer->fbo_id);
	framebuffer->bound = false;
	framebuffer->depthbuffer = 0;

	check_for_gl_errors(__FUNCTION__);
	return ref;
}

void attach_texture_as_colorbuffer(framebuffer_ref ref, const char *name, texture_ref texture) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	if (framebuffer->attachments_in_use == framebuffer->max_number_of_color_attachments) {
		fprintf(stderr, "Cannot attach yet another buffer (%s) (the %d th) to the fbo %s.", name, framebuffer->max_number_of_color_attachments, framebuffer->name);
		exit(-1);
	}
	int id = framebuffer->attachments_in_use++;
	framebuffer->color_attachment_names[id] = malloc(strlen(name)+1);
	strcpy(framebuffer->color_attachment_names[id], name);
	framebuffer->color_textures[id] = texture;
	printf("attaching texture id %d to framebuffer %s as att #%d.\n", texture_id(texture), framebuffer->name, id);
	framebuffer->color_attachments[id] = GL_COLOR_ATTACHMENT0 + id;
	glFramebufferTexture2D(GL_FRAMEBUFFER, framebuffer->color_attachments[id], GL_TEXTURE_2D, texture_id(texture), 0);
	check_for_gl_errors(__FUNCTION__);
}

void attach_texture_as_depthbuffer(framebuffer_ref ref, const char *name, texture_ref texture) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	framebuffer->depthbuffer_name = malloc(strlen(name)+1);
	framebuffer->depth_tex = texture;
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
#if CGL_GL_VERSION == GL3
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, framebuffer->width, framebuffer->height);
#else
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer->width, framebuffer->height);
#endif
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthbuffer);
	// unbind renderbuffer?
	check_for_gl_errors(__FUNCTION__);
}

void check_framebuffer_setup(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	if (!framebuffer->depthbuffer_name) {
		fprintf(stderr, "Framebuffer %s does not contain a depthbuffer.\n", framebuffer->name);
		exit(-1);
	}
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Frammebuffer %s is not complete. Errorcode %d\n", framebuffer->name, status);
		exit(-1);
	}
	check_for_gl_errors(__FUNCTION__);
}

static int old_vp[4];
void bind_framebuffer(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
		
	// well, for now...
	glGetIntegerv(GL_VIEWPORT, old_vp);
	glViewport(0,0, framebuffer->width, framebuffer->height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthbuffer);
#if CGL_GL_VERSION == GL3
	glDrawBuffers(framebuffer->attachments_in_use, framebuffer->color_attachments);
#endif
	framebuffer->bound = true;
}

void unbind_framebuffer(framebuffer_ref ref) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	framebuffer->bound = false;
	glViewport(old_vp[0], old_vp[1], old_vp[2],  old_vp[3]);
}

void resize_framebuffer(framebuffer_ref ref, unsigned int width, unsigned int height) {
	struct framebuffer *framebuffer = framebuffers+ref.id;
	framebuffer->width = width;
	framebuffer->height = height;
	for (int i = 0; i < framebuffer->attachments_in_use; ++i)
		resize_texture(framebuffer->color_textures[i], width, height);
	if (valid_texture_ref(framebuffer->depth_tex))
		resize_texture(framebuffer->depth_tex, width, height);
}

framebuffer_ref find_framebuffer(const char *name) {
	framebuffer_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_framebuffer_index; ++i)
		if (strcmp(framebuffers[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	return ref;
}

#ifdef WITH_GUILE
#include  <libguile.h>
#include <stdio.h>

SCM_DEFINE(s_make_framebuffer, "make-framebuffer", 3, 0, 0, (SCM n, SCM w, SCM h), "") {
	const char *name = scm_to_locale_string(n);
	int width = scm_to_int(w), height = scm_to_int(h);
	framebuffer_ref ref = make_framebuffer(name, width, height);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_attach_tex_as_cb, "attach-texture-as-colorbuffer", 3, 0, 0, (SCM tofbo, SCM n, SCM atex), "") {
	const char *name = scm_to_locale_string(n);
	framebuffer_ref fbo = { scm_to_int(tofbo) };
	texture_ref tex = { scm_to_int(atex) };
	attach_texture_as_colorbuffer(fbo, name, tex);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_attach_tex_as_db, "attach-texture-as-depthbuffer", 3, 0, 0, (SCM tofbo, SCM n, SCM atex), "") {
	const char *name = scm_to_locale_string(n);
	framebuffer_ref fbo = { scm_to_int(tofbo) };
	texture_ref tex = { scm_to_int(atex) };
	attach_texture_as_depthbuffer(fbo, name, tex);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_attach_db, "attach-depthbuffer", 1, 0, 0, (SCM tofbo), "") {
	framebuffer_ref fbo = { scm_to_int(tofbo) };
	attach_depth_buffer(fbo);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_check_fbo, "check-framebuffer-setup", 1, 0, 0, (SCM offbo), "") {
	framebuffer_ref fbo = { scm_to_int(offbo) };
	check_framebuffer_setup(fbo);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_bind_fbo, "bind-framebuffer", 1, 0, 0, (SCM afbo), "") {
	framebuffer_ref fbo = { scm_to_int(afbo) };
	bind_framebuffer(fbo);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_unbind_fbo, "unbind-framebuffer", 1, 0, 0, (SCM afbo), "") {
	framebuffer_ref fbo = { scm_to_int(afbo) };
	unbind_framebuffer(fbo);
	return SCM_BOOL_T;
}

void register_scheme_functions_for_framebuffers() {
#ifndef SCM_MAGIC_SNARFER
#include "framebuffer.x"
#endif
}

#endif
