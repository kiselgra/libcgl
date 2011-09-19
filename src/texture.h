#ifndef __TEXTURE_H__ 
#define __TEXTURE_H__ 

typedef struct {
	int id;
} texture_ref;


texture_ref make_texture(const char *name, const char *filename, int target);
int texture_id(texture_ref ref);

#endif

