#ifndef __SHADER_H__ 
#define __SHADER_H__ 

#include <stdbool.h>

typedef struct {
	int id;
} shader_ref;


shader_ref make_shader(const char *name, int input_vars);
void add_vertex_source(shader_ref ref, const char *src);
void add_fragment_source(shader_ref ref, const char *src);
void add_geometry_source(shader_ref ref, const char *src);
bool add_shader_input(shader_ref ref, const char *varname, unsigned int index);
bool modify_shader_input_index(shader_ref ref, const char *varname, unsigned int new_index);
bool compile_and_link_shader(shader_ref ref);
void bind_shader(shader_ref ref);
void unbind_shader(shader_ref ref);

const char* vertex_shader_info_log(shader_ref ref);
const char* fragment_shader_info_log(shader_ref ref);
const char* geometry_shader_info_log(shader_ref ref);
const char* shader_info_log(shader_ref ref);

int gl_shader_object(shader_ref ref);
shader_ref find_shader(const char *name);
bool valid_shader_ref(shader_ref ref);
shader_ref make_invalid_shader();
const char* shader_name(shader_ref ref);

#endif

