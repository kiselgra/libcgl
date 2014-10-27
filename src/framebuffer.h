#ifndef __FRAMEBUFFER_H__ 
#define __FRAMEBUFFER_H__ 

#include "texture.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int id;
} framebuffer_ref;

framebuffer_ref make_framebuffer(const char *name, unsigned int width, unsigned int height);
void attach_texture_as_colorbuffer(framebuffer_ref ref, const char *name, texture_ref texture);
void attach_texture_as_depthbuffer(framebuffer_ref ref, const char *name, texture_ref texture);
void attach_depth_buffer(framebuffer_ref ref);
void draw_buffers_done(framebuffer_ref ref);
void check_framebuffer_setup(framebuffer_ref ref);
void bind_framebuffer(framebuffer_ref ref);
void unbind_framebuffer(framebuffer_ref ref);
void resize_framebuffer(framebuffer_ref ref, unsigned int width, unsigned int height);
framebuffer_ref find_framebuffer(const char *name);
char* framebuffer_name(framebuffer_ref ref);
bool valid_framebuffer_ref(framebuffer_ref ref);
GLuint framebuffer_id(framebuffer_ref);

texture_ref framebuffer_depth_texture(framebuffer_ref ref);
texture_ref* framebuffer_color_textures(framebuffer_ref ref, int *n);

framebuffer_ref currently_bound_framebuffer();

define_array(framebuffer);

// fbo stack

framebuffer_ref current_framebuffer();
void pop_framebuffer();
void push_framebuffer(framebuffer_ref fbo);

#ifdef __cplusplus
}
#endif

#endif

