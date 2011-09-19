#include "texture.h"

#include <GL/glew.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct texture {
	char *name;
	GLuint texid;
	GLuint target;
	bool use_mipmapping;
	GLenum param_min, param_mag, param_wrap_s, param_wrap_t;
	unsigned int width, height;
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

texture_ref make_texture(const char *name, const char *filename, int target) {
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

	texture->width = 2;
	texture->height = 2;
// 	float data[2*2*4] = { 1,0,0,1,   0,1,0,1,   0,0,1,1,    0,1,1,1 };
// 	glTexImage2D(target, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_FLOAT, data);
	unsigned char data[2*2*4] = { 255,0,0,255,   0,255,0,255,  0,0,255,255,  0,255,255,255 };
	glTexImage2D(target, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(target, 0);

	return ref;
}

int texture_id(texture_ref ref) {
	return textures[ref.id].texid;
}

