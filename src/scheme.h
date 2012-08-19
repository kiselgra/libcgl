#ifndef __SCHEME_H__ 
#define __SCHEME_H__ 

#include <libmcm/vectors.h>
#include <libguile.h>

#ifdef __cplusplus
extern "C" {
#endif


void load_snarfed_definitions();
void start_console_thread();
void load_configfile(const char *filename);

unsigned int scheme_symbol_to_gl_enum(void *s);

vec3f list_to_vec3f(SCM v);
vec4f list_to_vec4f(SCM v);
SCM vec3f_to_list(vec3f *v);
SCM vec4f_to_list(vec4f *v);

#ifdef __cplusplus
}
#endif

#endif

