#ifndef __TEXTURE_H__ 
#define __TEXTURE_H__ 

#include <stdbool.h>

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


#ifdef HAVE_LIBPNG
texture_ref make_texture(const char *name, const char *filename, int target, tex_params_t *params);
texture_ref make_texture_ub(const char *name, const char *filename, int target, tex_params_t *params);
#endif
texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format, tex_params_t *params);
void resize_texture(texture_ref ref, unsigned int w, unsigned int h);
int texture_id(texture_ref ref);
void bind_texture(texture_ref ref, int unit);
void unbind_texture(texture_ref ref);
void save_texture_as_png(texture_ref ref, const char *filename);
void set_texture_params(texture_ref ref, tex_params_t *params);

unsigned int texture_height(texture_ref ref);
unsigned int texture_width(texture_ref ref);
const tex_params_t* texture_params(texture_ref ref);

bool valid_texture_ref(texture_ref ref);
texture_ref find_texture(const char *name);


#ifdef __cplusplus
}
#endif

#endif

