#ifndef __TEXTURE_H__ 
#define __TEXTURE_H__ 

typedef struct {
	int id;
} texture_ref;


texture_ref make_texture(const char *name, const char *filename, int target);
texture_ref make_empty_texture(const char *name, unsigned int w, unsigned int h, int target, unsigned int internal_format, unsigned int type, unsigned int format);
int texture_id(texture_ref ref);
void bind_texture(texture_ref ref);
void unbind_texture(texture_ref ref);
void save_texture_as_rgb_png(texture_ref ref, const char *filename);

#endif

