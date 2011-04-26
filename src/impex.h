#ifndef __IMPEX_H__ 
#define __IMPEX_H__ 

#include <libmcm-0.0.1/vectors.h>

#ifdef __cplusplus
extern "C" {
#endif


vec3f* load_png3f(const char *filename, unsigned int *w, unsigned int *h);
unsigned char* load_png3ub(const char *filename, unsigned int *w, unsigned int *h);

void save_png1f(const float *data, unsigned int w, unsigned int h, const char *filename);
void save_png3f(const vec3f *data, unsigned int w, unsigned int h, const char *filename);
void save_png4f(const vec4f *data, unsigned int w, unsigned int h, const char *filename);
	
void append_image_path(const char *path);
void prepend_image_path(const char *path);
bool file_exists(const char *name);
char* find_file(const char *basename);


#ifdef __cplusplus
}
#endif

#endif

