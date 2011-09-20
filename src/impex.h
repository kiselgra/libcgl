#ifndef __IMPEX_H__ 
#define __IMPEX_H__ 

#ifdef __cplusplus
extern "C" {
#endif

// c libraries go here!

#include <libmcm-0.0.1/vectors.h>


vec3f* load_png3f(const char *filename, unsigned int *w, unsigned int *h);
void save_png3f(const vec3f *data, unsigned int w, unsigned int h, const char *filename);
void save_png4f(const vec4f *data, unsigned int w, unsigned int h, const char *filename);

#ifdef __cplusplus
}
#endif

#endif

