#include "shader.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

struct shader {
	char *name;
	char *vertex_source,
	     *fragment_source;
	GLuint vertex_program, fragment_program;
	GLuint shader_program;
	char **input_var_names;
	unsigned int *input_var_ids;
	int input_vars;
	int next_input_var;
	bool bound, built_ok;
	char *vert_info_log, *frag_info_log, *program_info_log;
};

static struct shader *shaders = 0;
static unsigned int shaders_allocated = 0,
                    next_shader_index = 0;

shader_ref make_shader(const char *name, int input_vars) {
	if (next_shader_index >= shaders_allocated) {
		struct shader *old_array = shaders;
		unsigned int allocate = 1.5 * (shaders_allocated + 1);
		shaders = malloc(sizeof(struct shader) * allocate);
		for (int i = 0; i < shaders_allocated; ++i)
			shaders[i] = old_array[i];
		shaders_allocated = allocate;
		free(old_array);
	}
	shader_ref ref;
	ref.shader_id = next_shader_index++;
	struct shader *shader = shaders+ref.shader_id;
	shader->name = malloc(strlen(name)+1);
	strcpy(shader->name, name);
	shader->bound = false;
	shader->built_ok = false;
	shader->input_vars = input_vars;
	shader->next_input_var = 0;
	shader->vert_info_log = shader->frag_info_log = 0;
	shader->input_var_names = malloc(sizeof(char*) * input_vars);
	shader->input_var_ids = malloc(sizeof(unsigned int*) * input_vars);
	for (int i = 0; i < input_vars; ++i) {
		shader->input_var_names[i] = 0;
		shader->input_var_ids[i] = 0;
	}
	shader->vertex_source = shader->fragment_source = 0;
	shader->vertex_program = shader->fragment_program = shader->shader_program = 0;
	return ref;
}

void add_shader_source(char **destination, const char *add) {
	unsigned int old_size = 0;
	if (*destination) old_size = strlen(*destination) + 1; // we insert an additional \n
	unsigned int new_size = strlen(add);
	char *new_src = malloc(old_size + new_size+1);
	if (old_size) {
		strcpy(new_src, *destination);
		new_src[old_size] = '\n';
	}
	strcpy(new_src + old_size, add);
	free(*destination);
	*destination = new_src;
}

void add_vertex_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.shader_id;
	add_shader_source(&shader->vertex_source, src);
}

void add_fragment_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.shader_id;
	add_shader_source(&shader->fragment_source, src);
}

bool add_shader_input(shader_ref ref, const char *varname, unsigned int index) {
	struct shader *shader = shaders+ref.shader_id;
	if (shader->next_input_var >= shader->input_vars) {
		fprintf(stderr, "tried to assign shader input (%s, %d) to shader %s when all bindings are set.\n", varname, index, shader->name);
		return false;
	}
	int i = shader->next_input_var++;
	shader->input_var_names[i] = malloc(strlen(varname)+1);
	strcpy(shader->input_var_names[i], varname);
	shader->input_var_ids[i] = index;
	return true;
}

bool modify_shader_input_index(shader_ref ref, const char *varname, unsigned int new_index) {
	// TO DO.
	// note: shader must be re-linked afterwards.
	fprintf(stderr, "not supported yet.");
	return false;
}

void store_info_log(char **target, GLuint object) {
	int len = 0, written = 0;
	if (glIsShader(object))       glGetShaderiv(object, GL_INFO_LOG_LENGTH, &len);
	else if (glIsProgram(object)) glGetProgramiv(object, GL_INFO_LOG_LENGTH, &len);
	else                          len = 127;
	free(*target);
	*target = malloc(len+1);
	while (written < len-1) {
		printf("len = %d, written = %d\n", len, written);
		if (glIsShader(object))       glGetShaderInfoLog(object, len, &written, *target);
		else if (glIsProgram(object)) glGetProgramInfoLog(object, len, &written, *target);
		else                          strcpy(*target, "tried to get info log of an object which is neither a shader nor a program"), written = len;
	}
	(*target)[len] = '\0';
}

bool compile_and_link_shader(shader_ref ref) {
	struct shader *shader = shaders+ref.shader_id;
	const GLchar *src[2];
	GLint compile_res;

	// compile shader source
	shader->vertex_program = glCreateShader(GL_VERTEX_SHADER);
	shader->fragment_program = glCreateShader(GL_FRAGMENT_SHADER);
	src[0] = shader->vertex_source;
	src[1] = shader->fragment_source;
	glShaderSource(shader->vertex_program, 1, src, 0);
	glShaderSource(shader->fragment_program, 1, src+1, 0);
	glCompileShader(shader->vertex_program);
	glCompileShader(shader->fragment_program);

	glGetShaderiv(shader->vertex_program, GL_COMPILE_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->vert_info_log, shader->vertex_program);
		glDeleteShader(shader->vertex_program);
		glDeleteShader(shader->fragment_program);
		fprintf(stderr, "failed to compile vertex shader of %s\n", shader->name);
		return false;
	}

	glGetShaderiv(shader->fragment_program, GL_COMPILE_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->frag_info_log, shader->fragment_program);
		glDeleteShader(shader->vertex_program);
		glDeleteShader(shader->fragment_program);
		fprintf(stderr, "failed to compile fragment shader of %s\n", shader->name);
		return false;
	}

	shader->shader_program = glCreateProgram();
	glAttachShader(shader->shader_program, shader->vertex_program);
	glAttachShader(shader->shader_program, shader->fragment_program);

	// bind locations
	for (int i = 0; i < shader->input_vars; ++i) {
		if (shader->input_var_names[i] == 0) {
			fprintf(stderr, "not all shader inputs are set!\n");
			return false;
		}
		glBindAttribLocation(shader->shader_program, shader->input_var_ids[i], shader->input_var_names[i]);
	}

	// link
	glLinkProgram(shader->shader_program);
	glGetProgramiv(shader->shader_program, GL_LINK_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->program_info_log, shader->shader_program);
		fprintf(stderr, "faild to link shader %s\n", shader->name);
		return false;
	}
	
	shader->built_ok = true;
	return true;
}

void bind_shader(shader_ref ref) {
	glUseProgram(shaders[ref.shader_id].shader_program);
	shaders[ref.shader_id].bound = true;
}

void unbind_shader(shader_ref ref) {
	glUseProgram(0);
	shaders[ref.shader_id].bound = false;
}

const char *vertex_shader_info_log(shader_ref ref) {
	return shaders[ref.shader_id].vert_info_log;
}

const char *fragment_shader_info_log(shader_ref ref) {
	return shaders[ref.shader_id].frag_info_log;
}

const char *shader_info_log(shader_ref ref) {
	return shaders[ref.shader_id].program_info_log;
}

int gl_shader_object(shader_ref ref) {
	return shaders[ref.shader_id].shader_program;
}

#ifdef WITH_GUILE
#include <libguile.h>

SCM_DEFINE(s_make_shader, "make-shader", 2, 0, 0, (SCM name, SCM input_n), "create shader with name and number of input vars.") {
	char *na = scm_to_locale_string(name);
	int nu = scm_to_int(input_n);
	shader_ref ref = make_shader(na, nu);
	return scm_from_int(ref.shader_id);
}
SCM_DEFINE(s_add_vertex_source, "add-vertex-source", 2, 0, 0, (SCM shader, SCM src), "add vertex shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_vertex_source(ref, source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_fragment_source, "add-fragment-source", 2, 0, 0, (SCM shader, SCM src), "add fragment shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_fragment_source(ref, source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_shader_input, "add-shader-input", 3, 0, 0, (SCM shader, SCM varname, SCM index), "") {
	shader_ref ref = { scm_to_int(shader) };
	char *vn = scm_to_locale_string(varname);
	int idx = scm_to_int(index);
	bool ret = add_shader_input(ref, vn, idx);
	return ret ? SCM_BOOL_T : SCM_BOOL_F;
}
SCM_DEFINE(s_compile_and_link_shader, "compile-and-link-shader", 1, 0, 0, (SCM shader), "") {
	shader_ref ref = { scm_to_int(shader) };
	bool ret = compile_and_link_shader(ref);
	return ret ? SCM_BOOL_T : SCM_BOOL_F;
}

void register_scheme_functions_for_shaders() {
#ifndef SCM_MAGIC_SNARFER
#include "shader.x"
#endif
	printf("%s\n", s_s_make_shader);
}
#endif
