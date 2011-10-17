#ifndef __TEXTURE_H__ 
#define __TEXTURE_H__ 

#include <stdbool.h>

typedef struct {
	int id;
} texture_ref;


texture_ref make_texture(const char *name, const char *filename, int target, bool mipmap);
texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format);
int texture_id(texture_ref ref);
void bind_texture(texture_ref ref, int unit);
void unbind_texture(texture_ref ref);
void save_texture_as_png(texture_ref ref, const char *filename);

unsigned int texture_height(texture_ref ref);
unsigned int texture_width(texture_ref ref);

bool valid_texture_ref(texture_ref ref);
texture_ref find_texture(const char *name);

#endif

