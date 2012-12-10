#ifndef __SCHEME_H__ 
#define __SCHEME_H__ 

#ifdef WITH_GUILE

#include <libmcm/matrix.h>
#include <libmcm/vectors.h>
#include <libguile.h>

#ifdef __cplusplus
extern "C" {
#endif


void load_snarfed_definitions(void);
void start_console_thread(void);
void load_configfile(const char *filename);

unsigned int scheme_symbol_to_gl_enum(void *s);

vec3f list_to_vec3f(SCM v);
vec4f list_to_vec4f(SCM v);
SCM vec3f_to_list(vec3f *v);
SCM vec4f_to_list(vec4f *v);
SCM vec3f_to_scm_vec(vec3f *v);
SCM vec4f_to_scm_vec(vec4f *v);
vec3f scm_vec_to_vec3f(SCM v);
vec4f scm_vec_to_vec4f(SCM v);

SCM matrix4x4f_to_scm(matrix4x4f *m);
SCM scm_to_matrix4x4f(matrix4x4f *m, SCM bv);

#ifdef __cplusplus
}
#endif

#endif

#endif

