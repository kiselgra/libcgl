#ifndef __TEXTURE_H__ 
#define __TEXTURE_H__ 

#include <stdbool.h>
#include <libmcm/vectors.h>

#include "cgl-config.h"
#include "gl-version.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int id;
} texture_ref;

typedef struct {
	unsigned int min, mag, wrap_s, wrap_t;
	bool mipmapping;
} tex_params_t;


// the default params are: mipmapping on, repeat.
tex_params_t default_tex_params();
tex_params_t default_fbo_tex_params();


#if LIBCGL_HAVE_LIBPNG == 1
texture_ref make_texture(const char *name, const char *filename, int target, tex_params_t *params);
texture_ref make_texture_ub(const char *name, const char *filename, int target, tex_params_t *params);
#endif
texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params);
texture_ref make_empty_texture1d(const char *name, unsigned int elems, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params);
texture_ref make_buffer_texture(const char *name, unsigned int elements, unsigned int element_size, unsigned int internal_format);
void resize_texture(texture_ref ref, unsigned int w, unsigned int h);
int texture_id(texture_ref ref);
void bind_texture_as_image(texture_ref ref, int unit, int level, GLenum access, GLenum format);
void unbind_texture_as_image(texture_ref ref, int unit);
void bind_texture(texture_ref ref, int unit);
void unbind_texture(texture_ref ref);
void save_texture_as_png(texture_ref ref, const char *filename);
void set_texture_params(texture_ref ref, tex_params_t *params);

unsigned int texture_height(texture_ref ref);
unsigned int texture_width(texture_ref ref);
const tex_params_t* texture_params(texture_ref ref);

bool valid_texture_ref(texture_ref ref);
texture_ref find_texture(const char *name);
const char* texture_name(texture_ref ref);
GLuint texture_buffer(texture_ref ref);
texture_ref find_texture_by_filename(const char *filename);


void* download_texture(texture_ref ref, GLenum format, size_t bytes, GLenum type);
float* download_texture1f(texture_ref ref);
vec3f* download_texture3f(texture_ref ref);
vec4f* download_texture4f(texture_ref ref);

int* download_texture1i(texture_ref ref);

#ifdef __cplusplus
}
#endif

#endif

