#include "texture.h"
#include "impex.h"
#ifdef WITH_GUILE
#include "scheme.h"
#endif
#include "cgl.h"

#include <libmcm-0.0.1/vectors.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*! \defgroup textures Textures
 *
 *  The most common (in our experience) texture usages are covered by this, of course this is far from all.
 *  We support mainly 2D textures and buffer textures.
 *  They can be bound as samplers or images.
 *
 *  There is limited support for 1D textures.
 */

/*! \file texture.h
 *  \ingroup textures
 */

struct texture {
	char *name;
	GLuint texid;
	GLuint target;
	tex_params_t params;
	unsigned int width, height, samples;
	bool bound;
	GLenum internal_format,  // GL_RGBA32F ...
		   format,           // GL_RGB ...
		   type;             // GL_FLOAT ...
	GLuint buffer;
	char *filename;	//!< might be 0.
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

/*! \addtogroup textures
 *  @{
 */

tex_params_t default_tex_params() {
	tex_params_t p;
	p.mipmapping = true;
	p.min = GL_LINEAR_MIPMAP_LINEAR;
	p.mag = GL_LINEAR;
	p.wrap_s = GL_REPEAT;
	p.wrap_t = GL_REPEAT;
	return p;
}

tex_params_t default_fbo_tex_params() {
	tex_params_t p;
	p.mipmapping = false;
	p.min = GL_LINEAR;
	p.mag = GL_LINEAR;
	p.wrap_s = GL_REPEAT;
	p.wrap_t = GL_REPEAT;
	return p;
}

static void incorporate_params(struct texture *tex, tex_params_t *params) {
	if (params) tex->params = *params;
	else        tex->params = default_tex_params();
}

static texture_ref internal_make_tex(const char *name, GLenum target, tex_params_t *params, GLenum int_format, GLenum format, GLenum type, int w, int h, void *data) {
	texture_ref ref;
	allocate_texture();
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);
	
	glGenTextures(1, &texture->texid);
	
	glBindTexture(target, texture->texid);
	texture->target = target;
	texture->format = format; // data comes in this format
	texture->internal_format = int_format; // data is stored in this format
	texture->type = type; // data is expected to come as float
	texture->bound = true;           // ---v
	set_texture_params(ref, params); // does bind and unbind.
	texture->bound = false;
	texture->buffer = 0;
	texture->filename = 0;

	texture->width = w;
	texture->height = h;
	glTexImage2D(target, 0, texture->internal_format, texture->width, texture->height, 0, texture->format, texture->type, data);
	if (texture->params.mipmapping)
		glGenerateMipmap(target);

	glBindTexture(target, 0);
	check_for_gl_errors("after internal-make-texture");

	return ref;
}

#if LIBCGL_HAVE_LIBPNG == 1 || LIBCGL_HAVE_MAGICKWAND == 1
texture_ref make_texture_ub(const char *name, const char *filename, int target, tex_params_t *params) {
	unsigned int w, h;
	char *actual_name = find_file(filename);
	texture_ref ref;
	if (!actual_name) {
		fprintf(stderr, "File '%s' not found in any registered search directory.\n", filename);
		ref.id = -1;
		return ref;
	}
	unsigned char *data = load_png3ub(actual_name, &w, &h);
	free(actual_name);
	
	ref = internal_make_tex(name, target, params, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, w, h, data);
	free(data);
	
	textures[ref.id].filename = strdup(filename);

	check_for_gl_errors(__FUNCTION__);
	return ref;
}

/*! \brief Load a texture from file.
 *  \ingroup textures
 *  \note does not work on GLES.
 *  \note alpha is discarded.
 */
texture_ref make_texture(const char *name, const char *filename, int target, tex_params_t *params) {
	unsigned int w, h;
	char *actual_name = find_file(filename);
	texture_ref ref;
	if (!actual_name) {
		fprintf(stderr, "File '%s' not found in any registered search directory.\n", filename);
		ref.id = -1;
		return ref;
	}
// 	vec3f *data = load_png3f(actual_name, &w, &h);
// 	vec3f *data = load_image3f(actual_name, &w, &h);
	vec4f *data = load_image4f(actual_name, &w, &h);
	free(actual_name);
	
// 	ref = internal_make_tex(name, target, params, GL_RGBA, GL_RGB, GL_FLOAT, w, h, data);
	ref = internal_make_tex(name, target, params, GL_RGBA, GL_RGBA, GL_FLOAT, w, h, data);
	free(data);

	textures[ref.id].filename = strdup(filename);

	check_for_gl_errors(__FUNCTION__);
	return ref;
}
#endif

/*! \brief Creates an empty texture.
 *  \ingroup textures
 *  \note The parameter order of int-format, type, format is not the same as in the glTexImage2D call.
 *  \param s the number of samples per pixel. use 1 to specify non-multisample texture setup.
 *  \note For MS you need to specify a multisampling texture target, e.g. GL_TEXTURE_2D_MULTISAMPLE
 */
texture_ref make_empty_texture_ms(const char *name, unsigned int w, unsigned int h, unsigned int s, int target, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params) {
	allocate_texture();
	texture_ref ref;
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);

	glGenTextures(1, &texture->texid);
	texture->target = target;
	glBindTexture(target, texture->texid);
	
	texture->bound = true;
	set_texture_params(ref, params);
	texture->bound = false;

	texture->width = w;
	texture->height = h;
	texture->format = format;
	texture->internal_format = internal_format;
	texture->type = type;
	texture->buffer = 0;
	texture->filename = 0;
	texture->samples = s;
	if (s <= 1)
		glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, type, 0);
	else
		glTexImage2DMultisample(target, s, internal_format, w, h, 0);

	glBindTexture(target, 0);
	check_for_gl_errors(__FUNCTION__);
	return ref;
}

/*! \brief Creates an empty texture.
 *  \ingroup textures
 *  \note The parameter order of int-format, type, format is not the same as in the glTexImage2D call.
 */
texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params) {
	return make_empty_texture_ms(name, w, h, 1, target, internal_format, type, format, params);
}

#if CGL_GL == GL
texture_ref make_empty_texture1d(const char *name, unsigned int elems, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params) {
	allocate_texture();
	texture_ref ref;
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);

	glGenTextures(1, &texture->texid);
	texture->target = GL_TEXTURE_1D;
	glBindTexture(GL_TEXTURE_1D, texture->texid);
	
	texture->bound = true;
	set_texture_params(ref, params);
	texture->bound = false;

	texture->width = elems;
	texture->height = 1;
	texture->format = format;
	texture->internal_format = internal_format;
	texture->type = type;
	texture->buffer = 0;
	texture->filename = 0;
	glTexImage1D(GL_TEXTURE_1D, 0, internal_format, texture->width, 0, format, type, 0);
	check_for_gl_errors("ti");

	glBindTexture(GL_TEXTURE_1D, 0);
	check_for_gl_errors(__FUNCTION__);
	return ref;
}

/*! \brief Creates a buffer and uses it as backing storage for the newly created texture.
 *  \ingroup textures
 */
texture_ref make_buffer_texture(const char *name, unsigned int elements, unsigned int element_size, unsigned int internal_format) {
	allocate_texture();
	texture_ref ref;
	ref.id = next_texture_index++;
	struct texture *texture = textures+ref.id;
	texture->name = malloc(strlen(name)+1);
	strcpy(texture->name, name);
	texture->filename = 0;

	glGenBuffers(1, &texture->buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, texture->buffer);

	unsigned int size = elements * element_size;
	glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_DYNAMIC_DRAW);

	texture->width = elements;
	texture->height = 1;
	texture->target = GL_TEXTURE_BUFFER;

	glGenTextures(1, &texture->texid);
	glBindTexture(GL_TEXTURE_BUFFER, texture->texid);
	glTexBuffer(GL_TEXTURE_BUFFER, internal_format, texture->buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return ref;
}
#endif

void set_texture_params(texture_ref ref, tex_params_t *params) {
	struct texture *texture = textures+ref.id;
	if (texture->samples <= 1) {
		bool was_bound = false;
		if (!texture->bound) glBindTexture(texture->target, texture->texid);
		else                 was_bound = true;
		incorporate_params(texture, params);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, texture->params.wrap_s);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, texture->params.wrap_t);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, texture->params.mag);
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, texture->params.min);
		if (!was_bound)  glBindTexture(texture->target, 0);
	}
}

/*! \note This does destroy the texture's content, yea?
 *  \ingroup textures
 */
void resize_texture(texture_ref ref, unsigned int w, unsigned int h) {
	struct texture *texture = textures + ref.id;
	glBindTexture(texture->target, texture->texid);
	texture->width = w;
	texture->height = h;
	glTexImage2D(texture->target, 0, texture->internal_format, w, h, 0, texture->format, texture->type, 0);
	glBindTexture(texture->target, 0);
	check_for_gl_errors(__FUNCTION__);
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

#if CGL_GL == GL
void bind_texture_as_image(texture_ref ref, int unit, int level, GLenum access, GLenum format) {
	struct texture *texture = textures + ref.id;
    texture->bound = true;
    glBindImageTexture(unit, texture->texid, 0, GL_FALSE, 0, access, format);
    check_for_gl_errors(__FUNCTION__);
}

void unbind_texture_as_image(texture_ref ref, int unit) {
	struct texture *texture = textures + ref.id;
    texture->bound = false;
    glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    check_for_gl_errors(__FUNCTION__);
}
#endif

void save_texture_as_png(texture_ref ref, const char *filename) {
#if CGL_GL == GL
	if (!valid_texture_ref(ref)) {
		fprintf(stderr, "cannot save. invalid texref (destination was %s)\n", filename);
		return;
	}
	struct texture *texture = textures + ref.id;
	bool was_bound = false;
	if (!texture->bound)
		bind_texture(ref, 0);
	else
		was_bound = true;

	if (texture->format == GL_RGB) {
		vec3f *data = malloc(sizeof(vec3f)*texture->width*texture->height);
		glGetTexImage(texture->target, 0, GL_RGB, GL_FLOAT, data);
		save_png3f(data, texture->width, texture->height, filename);
		free(data);
	}
	else if (texture->format == GL_RGBA) {
		vec4f *data = malloc(sizeof(vec4f)*texture->width*texture->height);
		glGetTexImage(texture->target, 0, GL_RGBA, GL_FLOAT, data);
		save_png4f(data, texture->width, texture->height, filename);
		free(data);
	}
	else if (texture->format == GL_RED) {
		float *data = malloc(sizeof(float)*texture->width*texture->height);
		glGetTexImage(texture->target, 0, GL_RED, GL_FLOAT, data);
		save_png1f(data, texture->width, texture->height, filename);
		free(data);
	}
	else if (texture->format == GL_DEPTH_COMPONENT) {
		float *data = malloc(sizeof(float)*texture->width*texture->height);
		glGetTexImage(texture->target, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		save_png1f(data, texture->width, texture->height, filename);
		free(data);
	}
	else if (texture->internal_format == GL_R32F) {
		float *data = malloc(sizeof(float)*texture->width*texture->height);
		glGetTexImage(texture->target, 0, GL_RED, GL_FLOAT, data);
		save_png1f(data, texture->width, texture->height, filename);
		free(data);
	}
	else fprintf(stderr, "Don't know how to save texture of format %ud.\n", texture->format);

	if (!was_bound)
		unbind_texture(ref);
	check_for_gl_errors(__FUNCTION__);
#else
	fprintf(stderr, "Cannot download textures on gles, yet.\n");
#endif
}

void* download_texture_to(texture_ref ref, GLenum format, size_t bytes, GLenum type, void *data) {
#if CGL_GL == GL
	struct texture *texture = textures + ref.id;
	bool was_bound = false;
	if (!texture->bound)
		bind_texture(ref, 0);
	else
		was_bound = true;

	glGetTexImage(texture->target, 0, format, type, data);

	if (!was_bound)
		unbind_texture(ref);
	check_for_gl_errors(__FUNCTION__);
	
	return data;
#else
	fprintf(stderr, "Cannot download textures on gles, yet.\n");
#endif
}

void* download_texture(texture_ref ref, GLenum format, size_t bytes, GLenum type) {
	struct texture *texture = textures + ref.id;
	void *data = malloc(bytes*texture->width*texture->height);
	return download_texture_to(ref, format, bytes, type, data);
}

#if CGL_GL == GL
float* download_texture1f(texture_ref ref) { return download_texture(ref, GL_RED,  sizeof(float), GL_FLOAT); }
vec3f* download_texture3f(texture_ref ref) { return download_texture(ref, GL_RGB,  sizeof(vec3f), GL_FLOAT); }
vec4f* download_texture4f(texture_ref ref) { return download_texture(ref, GL_RGBA, sizeof(vec4f), GL_FLOAT); }

int* download_texture1i(texture_ref ref) { return download_texture(ref, GL_RED_INTEGER,  sizeof(int), GL_INT); }

float* download_texture1f_to(texture_ref ref, float *data) { return download_texture_to(ref, GL_RED,  sizeof(float), GL_FLOAT, data); }
vec3f* download_texture3f_to(texture_ref ref, vec3f *data) { return download_texture_to(ref, GL_RGB,  sizeof(vec3f), GL_FLOAT, data); }
vec4f* download_texture4f_to(texture_ref ref, vec4f *data) { return download_texture_to(ref, GL_RGBA, sizeof(vec4f), GL_FLOAT, data); }
#endif

int texture_id(texture_ref ref) {
	return textures[ref.id].texid;
}

unsigned int texture_width(texture_ref ref) {
	return textures[ref.id].width;
}

unsigned int texture_height(texture_ref ref) {
	return textures[ref.id].height;
}

unsigned int texture_samples(texture_ref ref) {
	return textures[ref.id].samples;
}

const tex_params_t* texture_params(texture_ref ref) {
	return &textures[ref.id].params;
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

/*! \brief Find the that was loaded from the file \c filename. 
 *  Returns an invalid_ref if that file was never loaded.
 */
texture_ref find_texture_by_filename(const char *filename) {
	texture_ref ref = { -1 };
	if (strlen(filename) == 0) return ref;
// 	printf("looking for %s...\n", filename);
	for (int i = 0; i < next_texture_index; ++i) {
// 		if (textures[i].filename)
// 			printf("    checking %s...\n", textures[i].filename);
		if (textures[i].filename && strcmp(textures[i].filename, filename) == 0) {
			ref.id = i;
			return ref;
		}
	}
	return ref;
}

//! \attention might be 0!
const char* texture_source_filename(texture_ref ref) {
	return textures[ref.id].filename;
}

const char* texture_name(texture_ref ref) {
	return textures[ref.id].name;
}

bool valid_texture_ref(texture_ref ref) {
	return ref.id >= 0;
}

GLuint texture_buffer(texture_ref ref) {
	return textures[ref.id].buffer;
}

/*! @} */

#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>

#if LIBCGL_HAVE_LIBPNG == 1
SCM_DEFINE(s_make_texture_from_file, "texture-from-file", 4, 0, 0, (SCM name, SCM filename, SCM target, SCM mm), "") {
	char *n = scm_to_locale_string(name);
	char *fn = scm_to_locale_string(filename);
	GLenum t = scheme_symbol_to_gl_enum(&target);
	bool mipmap = scm_to_bool(mm);
	tex_params_t p;
	if (mipmap) p = default_tex_params();
	else        p = default_fbo_tex_params();
// 	printf("calling mt with %s %s\n", fn, gl_enum_string(t));
	texture_ref ref = make_texture(n, fn, t, &p);
	free(n);
	free(fn);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_make_texture_from_file_ub, "texture-from-file-ub", 4, 0, 0, (SCM name, SCM filename, SCM target, SCM mm), "") {
	char *n = scm_to_locale_string(name);
	char *fn = scm_to_locale_string(filename);
	GLenum t = scheme_symbol_to_gl_enum(&target);
	bool mipmap = scm_to_bool(mm);
	tex_params_t p;
	if (mipmap) p = default_tex_params();
	else        p = default_fbo_tex_params();
// 	printf("calling mt with %s %s\n", fn, gl_enum_string(t));
	texture_ref ref = make_texture_ub(n, fn, t, &p);
	free(n);
	free(fn);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_save_texture_as_png, "save-texture/png", 2, 0, 0, (SCM id, SCM to), "") {
    texture_ref ref = { scm_to_int(id) };
	char *path = scm_to_locale_string(to);
    save_texture_as_png(ref, path);
    free(path);
    return SCM_BOOL_T;
}

#endif

SCM_DEFINE(s_make_empty_texture, "make-texture-without-file", 7, 0, 0, (SCM name, SCM trg, SCM w, SCM h, SCM f, SCM inf, SCM ty), "") {
	char *n = scm_to_locale_string(name);
	unsigned int t = -1;
	GLenum target = scheme_symbol_to_gl_enum(&trg);
	GLenum format = scheme_symbol_to_gl_enum(&f);
	GLenum int_format = scheme_symbol_to_gl_enum(&inf);
	GLenum type = scheme_symbol_to_gl_enum(&ty);
	unsigned int width = scm_to_uint(w),
				 height = scm_to_uint(h);
	tex_params_t p = default_fbo_tex_params();
// 	printf("make texture %s with t=%s   f=%s   if=%s   ty=%s   w=%d   h=%d\n", n, gl_enum_string(target), gl_enum_string(format), gl_enum_string(int_format), gl_enum_string(type), width, height);
	texture_ref ref = make_empty_texture(n, width, height, target, int_format, type, format, &p);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_make_empty_texture1d, "make-texture-1d-without-file", 5, 0, 0, (SCM name, SCM w, SCM f, SCM inf, SCM ty), "") {
	char *n = scm_to_locale_string(name);
	GLenum format = scheme_symbol_to_gl_enum(&f);
	GLenum int_format = scheme_symbol_to_gl_enum(&inf);
	GLenum type = scheme_symbol_to_gl_enum(&ty);
	unsigned int width = scm_to_uint(w);
	tex_params_t p = default_fbo_tex_params();
	texture_ref ref = make_empty_texture1d(n, width, int_format, type, format, &p);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_set_tex_params, "tex-params!", 6, 0, 0, (SCM id, SCM mm, SCM min, SCM mag, SCM wrap_s, SCM wrap_t), "") {
	texture_ref ref = { scm_to_int(id) };
	tex_params_t p;
	p.mipmapping = scm_to_bool(mm);
	p.mag = scheme_symbol_to_gl_enum(&mag);
	p.min = scheme_symbol_to_gl_enum(&min);
	p.wrap_s = scheme_symbol_to_gl_enum(&wrap_s);
	p.wrap_t = scheme_symbol_to_gl_enum(&wrap_t);
	set_texture_params(ref, &p);
	return id;
}

SCM_DEFINE(s_bind_tex, "bind-texture", 2, 0, 0, (SCM id, SCM unit), "") {
	texture_ref ref = { scm_to_int(id) };
	bind_texture(ref, scm_to_int(unit));
	return scm_from_unsigned_integer(texture_width(ref));
}

SCM_DEFINE(s_unbind_tex, "unbind-texture", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	unbind_texture(ref);
	return scm_from_unsigned_integer(texture_width(ref));
}

SCM_DEFINE(s_bind_texture_as_image, "bind-texture-as-image", 5, 0, 0, (SCM id, SCM unit, SCM level, SCM access, SCM format), "") {
    texture_ref ref = { scm_to_int(id) };
    unsigned int u = scm_to_uint(unit),
                 l = scm_to_uint(level),
                 a = scm_to_uint(access),
                 f = scm_to_uint(format);
    bind_texture_as_image(ref, u, l, a, f);
    return SCM_BOOL_T;
}

SCM_DEFINE(s_unbind_texture_as_image, "unbind-texture-as-image", 2, 0, 0, (SCM id, SCM unit), "") {
    texture_ref ref = { scm_to_int(id) };
    unsigned int u = scm_to_uint(unit);
    unbind_texture_as_image(ref, u);
    return SCM_BOOL_T;
}

SCM_DEFINE(s_texture_w, "texture-width", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	return scm_from_unsigned_integer(texture_width(ref));
}

SCM_DEFINE(s_texture_h, "texture-height", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	return scm_from_unsigned_integer(texture_height(ref));
}

SCM_DEFINE(s_texture_name, "texture-name", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	return scm_from_locale_string(texture_name(ref));
}

SCM_DEFINE(s_find_texture, "find-texture", 1, 0, 0, (SCM name), "") {
	char *n = scm_to_locale_string(name);
	texture_ref ref = find_texture(n);
	free(n);
	if (valid_texture_ref(ref))
		return scm_from_int(ref.id);
	return SCM_BOOL_F;
}

SCM_DEFINE(s_download_tex1f, "download-texture1f", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	int bytes = texture_width(ref) * texture_height(ref) * sizeof(float);
	SCM bv = scm_c_make_bytevector(bytes);
	download_texture1f_to(ref, (float*)SCM_BYTEVECTOR_CONTENTS(bv));
	return bv;
}

SCM_DEFINE(s_download_tex3f, "download-texture3f", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	int bytes = texture_width(ref) * texture_height(ref) * sizeof(vec3f);
	SCM bv = scm_c_make_bytevector(bytes);
	download_texture3f_to(ref, (vec3f*)SCM_BYTEVECTOR_CONTENTS(bv));
	return bv;
}

SCM_DEFINE(s_download_tex4f, "download-texture4f", 1, 0, 0, (SCM id), "") {
	texture_ref ref = { scm_to_int(id) };
	int bytes = texture_width(ref) * texture_height(ref) * sizeof(vec4f);
	SCM bv = scm_c_make_bytevector(bytes);
	download_texture4f_to(ref, (vec4f*)SCM_BYTEVECTOR_CONTENTS(bv));
	return bv;
}

void register_scheme_functions_for_textures() {
#ifndef SCM_MAGIC_SNARFER
#include "texture.x"
#endif
}
#endif
