#include "texture.h"
#include "impex.h"
#include "cgl.h"

#include <libmcm-0.0.1/vectors.h>

#include <GL/glew.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct texture {
	char *name;
	GLuint texid;
	GLuint target;
	bool use_mipmapping;
	GLenum param_min, param_mag, param_wrap_s, param_wrap_t;
	unsigned int width, height;
	bool bound;
};

static struct texture *textures = 0;
static unsigned int textures_allocated = 0, 
                    next_texture_index = 0;

static void allocate_texture() {
	// maintain texture table
	if (next_texture_index >= textures_allocated) {
		struct texture *old_array = textures;
		unsigned int allocate = 1.5 * (textures_allocated + 1);
		textures = malloc(sizeof(struct texture) * allocate);
		for (int i = 0; i < textures_allocated; ++i)
			textures[i] = old_array[i];
		textures_allocated = allocate;
		free(old_array);
	}
}

texture_ref make_texture(const char *name, const char *filename, int target, bool mipmap) {
	unsigned int w, h;
	char *actual_name = find_file(filename);
	texture_ref ref;
	if (!actual_name) {
		fprintf(stderr, "File '%s' not found in any registered search directory.\n", filename);
		ref.id = -1;
		return ref;
	}
	vec3f *data = load_png3f(actual_name, &w, &h);
	free(actual_name);
	
	allocate_texture();
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);
	
	glGenTextures(1, &texture->texid);
	texture->target = target;
	glBindTexture(target, texture->texid);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (mipmap) {
	check_for_gl_errors("000");
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	check_for_gl_errors("aaa");	
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	check_for_gl_errors("bbb");
	}
	else {
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	texture->bound = false;

	texture->width = w;
	texture->height = h;
	glTexImage2D(target, 0, GL_RGBA8, texture->width, texture->height, 0, GL_RGB, GL_FLOAT, data);
	if (mipmap)
		glGenerateMipmap(target);

	glBindTexture(target, 0);

	free(data);

	check_for_gl_errors(__FUNCTION__);
	return ref;
}

texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format) {
	allocate_texture();
	texture_ref ref;
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);
	
	glGenTextures(1, &texture->texid);
	texture->target = target;
	glBindTexture(target, texture->texid);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture->bound = false;

	texture->width = w;
	texture->height = h;
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, type, 0);

	glBindTexture(target, 0);
	check_for_gl_errors(__FUNCTION__);
	return ref;
}

void bind_texture(texture_ref ref, int unit) {
	textures[ref.id].bound = true; 
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(textures[ref.id].target, textures[ref.id].texid); 
}

void unbind_texture(texture_ref ref) { 
	textures[ref.id].bound = false; 
	glBindTexture(textures[ref.id].target, 0); 
}

void save_texture_as_rgb_png(texture_ref ref, const char *filename) {
	struct texture *texture = textures + ref.id;
	bool was_bound = false;
	if (!texture->bound)
		bind_texture(ref, 0);
	else
		was_bound = true;

	/*
	vec4f *data = malloc(sizeof(vec4f)*texture->width*texture->height);
	glGetTexImage(texture->target, 0, GL_RGBA, GL_FLOAT, data);
	save_png4f(data, texture->width, texture->height, filename);
	*/
	vec3f *data = malloc(sizeof(vec3f)*texture->width*texture->height);
	glGetTexImage(texture->target, 0, GL_RGB, GL_FLOAT, data);
	save_png3f(data, texture->width, texture->height, filename);

	if (!was_bound)
		unbind_texture(ref);
	check_for_gl_errors(__FUNCTION__);
}

int texture_id(texture_ref ref) {
	return textures[ref.id].texid;
}

unsigned int texture_width(texture_ref ref) {
	return textures[ref.id].width;
}

unsigned int texture_height(texture_ref ref) {
	return textures[ref.id].height;
}

texture_ref find_texture(const char *name) {
	texture_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_texture_index; ++i)
		if (strcmp(textures[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	return ref;
}

bool valid_texture_ref(texture_ref ref) {
	return ref.id >= 0;
}

#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>

SCM_DEFINE(s_make_texture_from_file, "texture-from-file", 4, 0, 0, (SCM name, SCM filename, SCM target, SCM mm), "") {
	char *n = scm_to_locale_string(name);
	char *fn = scm_to_locale_string(filename);
	unsigned int t = -1;
	if (scm_is_symbol(target)) {
		char *n = scm_to_locale_string(scm_symbol_to_string(target));
		if (strcmp("tex2d", n) == 0) t = GL_TEXTURE_2D;
		else fprintf(stderr, "ERROR: unknown tex target '%s' at scheme level.\n", n);
	}
	else 
		t = scm_to_uint32(target);
	bool mipmap = scm_to_bool(mm);
	texture_ref ref = make_texture(n, fn, t, mm);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_texture_w, "texture-width", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	return scm_from_unsigned_integer(texture_width(ref));
}

SCM_DEFINE(s_texture_h, "texture-height", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	return scm_from_unsigned_integer(texture_height(ref));
}

void register_scheme_functions_for_textures() {
#ifndef SCM_MAGIC_SNARFER
#include "texture.x"
#endif
}
#endif
