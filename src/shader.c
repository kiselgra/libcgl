#include "cgl.h"
#include "shader.h"
#include "gl-version.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* A note on consistent shader reloading.
 *
 * When a shader is deleted it's index-slot is free to take by another shader.
 * As long as the list of free indices is not empty the next shader will be
 * allocated the index put last on the list.
 * This means it is safe to assume a shader keeps its id when reload is
 * implemented as follows:
 *   shader_ref old_ref = find_shader("my-shader");
 *   destroy_shader(old_ref);
 *   shader_ref new_ref = make_shader("my-shader", ...);
 *   --> old_ref.id == new_ref.id
 */


struct shader {
	char *name;
	char *vertex_source,
	     *fragment_source,
		 *geometry_source;
	GLuint vertex_program, fragment_program, geometry_program;
	GLuint shader_program;
	char **input_var_names;
	unsigned int *input_var_ids;
	int input_vars;
	int next_input_var;
	char **uniform_names;
	int *uniform_locations;
	int uniforms;
	int next_uniform_id;
	bool bound, built_ok;
	char *vert_info_log, *frag_info_log, *geom_info_log, *program_info_log;
};

#define TYPE shader
#define ARRAY shaders
#define REF shader_ref
#include "mm.h"

/*
static struct shader *shaders = 0;
static unsigned int shaders_allocated = 0,
                    next_shader_index = 0;

struct free_entry {
	int id;
	struct free_entry *next;
};
static struct free_entry *free_list = 0;

void put_on_free_list(int id) {
	struct free_entry *old = free_list;
	free_list = malloc(sizeof(struct free_entry));
	free_list->next = old;
	free_list->id = id;
}

int get_from_free_list() {
	if (!free_list) return -1;
	int ret = free_list->id;
	struct free_list *old = free_list;
	free_list = free_list->next;
	free(old);
	return ret;
}

bool element_available_on_free_list() { return free_list != 0; }
*/

shader_ref make_shader(const char *name, int input_vars, int uniforms) {
	printf("--> %s %d %d\n", name, input_vars, uniforms);
	/*
	shader_ref ref;
	if (element_available_on_free_list())
		ref.id = get_from_free_list();
	else {
		if (next_shader_index >= shaders_allocated) {
			struct shader *old_array = shaders;
			unsigned int allocate = 1.5 * (shaders_allocated + 1);
			shaders = malloc(sizeof(struct shader) * allocate);
			for (int i = 0; i < shaders_allocated; ++i)
				shaders[i] = old_array[i];
			shaders_allocated = allocate;
			free(old_array);
		}
		ref.id = next_shader_index++;
	}
	*/
	shader_ref ref = allocate_ref();
	struct shader *shader = shaders+ref.id;
	shader->name = malloc(strlen(name)+1);
	strcpy(shader->name, name);
	shader->bound = false;
	shader->built_ok = false;
	shader->input_vars = input_vars;
	shader->next_input_var = 0;
	shader->vert_info_log = shader->frag_info_log = shader->geom_info_log = shader->program_info_log = 0;
	shader->input_var_names = malloc(sizeof(char*) * input_vars);
	shader->input_var_ids = malloc(sizeof(unsigned int*) * input_vars);
	for (int i = 0; i < input_vars; ++i) {
		shader->input_var_names[i] = 0;
		shader->input_var_ids[i] = 0;
	}
	shader->uniforms = uniforms;
	shader->next_uniform_id = 0;
	shader->uniform_names = malloc(sizeof(char*) * uniforms);
	shader->uniform_locations = malloc(sizeof(int) * uniforms);
	for (int i = 0; i < uniforms; ++i) {
		shader->uniform_names[i] = 0;
		shader->uniform_locations[i] = -1; // invalid uniform (glGetUniformLocation)
	}
	shader->vertex_source = shader->fragment_source = shader->geometry_source = 0;
	shader->vertex_program = shader->fragment_program = shader->geometry_program = shader->shader_program = 0;
	return ref;
}

void destroy_shader(shader_ref ref) {
	if (!valid_shader_ref(ref)) return;
	struct shader *shader = shaders+ref.id;
	for (int i = 0; i < shader->next_input_var; ++i) {
		free(shader->input_var_names[i]);
	}
	free(shader->input_var_names);               shader->input_var_names = 0;
	free(shader->input_var_ids);                 shader->input_var_ids = 0;
	free(shader->name);                          shader->name = 0;
	free(shader->vertex_source);                 shader->vertex_source = 0;
	free(shader->fragment_source);               shader->fragment_source = 0;
	free(shader->geometry_source);               shader->geometry_source = 0;
	glDeleteProgram(shader->shader_program);     shader->shader_program = 0;
	glDeleteShader(shader->vertex_program);      shader->vertex_program = 0;
	glDeleteShader(shader->fragment_program);    shader->fragment_program = 0;
	glDeleteShader(shader->geometry_program);    shader->geometry_program = 0;
	free_ref(ref);
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
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->vertex_source, src);
}

void add_fragment_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->fragment_source, src);
}

void add_geometry_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->geometry_source, src);
}

bool add_shader_input(shader_ref ref, const char *varname, unsigned int index) {
	struct shader *shader = shaders+ref.id;
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

bool add_shader_uniform(shader_ref ref, const char *name) {
	struct shader *shader = shaders+ref.id;
	if (shader->next_uniform_id >= shader->uniforms) {
		fprintf(stderr, "tried to assign shader uniform (%s, %d) to shader %s when all bindings are set.\n", name, shader->next_uniform_id, shader->name);
		return false;
	}
	int i = shader->next_uniform_id++;
	shader->uniform_names[i] = malloc(strlen(name)+1);
	strcpy(shader->uniform_names[i], name);
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
	struct shader *shader = shaders+ref.id;
	const GLchar *src[3];
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
	
#if CGL_GL_VERSION == GL3
	if (shader->geometry_source) {
		shader->geometry_program = glCreateShader(GL_GEOMETRY_SHADER);
		src[2] = shader->geometry_source;
		glShaderSource(shader->geometry_program, 1, src+2, 0);
		glCompileShader(shader->geometry_program);
	}
#endif

	glGetShaderiv(shader->vertex_program, GL_COMPILE_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->vert_info_log, shader->vertex_program);
		glDeleteShader(shader->vertex_program);
		glDeleteShader(shader->fragment_program);
		if (shader->geometry_source) glDeleteShader(shader->geometry_program);
		fprintf(stderr, "failed to compile vertex shader of %s\n", shader->name);
		return false;
	}

	glGetShaderiv(shader->fragment_program, GL_COMPILE_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->frag_info_log, shader->fragment_program);
		glDeleteShader(shader->vertex_program);
		glDeleteShader(shader->fragment_program);
		if (shader->geometry_source) glDeleteShader(shader->geometry_program);
		fprintf(stderr, "failed to compile fragment shader of %s\n", shader->name);
		return false;
	}

#if CGL_GL_VERSION == GL3
	if (shader->geometry_source) {
		glGetShaderiv(shader->geometry_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->geom_info_log, shader->geometry_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			glDeleteShader(shader->geometry_program);
			fprintf(stderr, "failed to compile geometry shader of %s\n", shader->name);
			return false;
		}
	}
#endif

	shader->shader_program = glCreateProgram();
	glAttachShader(shader->shader_program, shader->vertex_program);
	glAttachShader(shader->shader_program, shader->fragment_program);
#if CGL_GL_VERSION == GL3
	if (shader->geometry_program) glAttachShader(shader->shader_program, shader->geometry_program);
#endif

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

	for (int i = 0; i < shader->uniforms; ++i) {
		int loc = glGetUniformLocation(shader->shader_program, shader->uniform_names[i]);
		if (loc < 0)
			fprintf(stderr, "WARNING: Location of uniform %s in shader %s is < 0.\n", shader->uniform_names[i], shader->name);
		shader->uniform_locations[i] = loc;
	}

	return true;
}

void bind_shader(shader_ref ref) {
	glUseProgram(shaders[ref.id].shader_program);
	shaders[ref.id].bound = true;
}

void unbind_shader(shader_ref ref) {
	glUseProgram(0);
	shaders[ref.id].bound = false;
}

const char *vertex_shader_info_log(shader_ref ref) {
	return shaders[ref.id].vert_info_log;
}

const char *fragment_shader_info_log(shader_ref ref) {
	return shaders[ref.id].frag_info_log;
}

const char *geometry_shader_info_log(shader_ref ref) {
	return shaders[ref.id].geom_info_log;
}

const char *shader_info_log(shader_ref ref) {
	return shaders[ref.id].program_info_log;
}

int gl_shader_object(shader_ref ref) {
	return shaders[ref.id].shader_program;
}

shader_ref find_shader(const char *name) {
	shader_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_index; ++i) {
		if (strcmp(shaders[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	}
	return ref;
}

bool valid_shader_ref(shader_ref ref) {
	return ref.id >= 0;
}

shader_ref make_invalid_shader(void) {
	shader_ref r = { -1 };
	return r;
}

const char* shader_name(shader_ref ref) {
	return shaders[ref.id].name;
}

int shader_uniforms(shader_ref ref) {
	struct shader *shader = shaders+ref.id;
	return shader->uniforms;
}

const char* shader_uniform_name_by_id(shader_ref ref, unsigned int id) {
	struct shader *shader = shaders+ref.id;
	return shader->uniform_names[id];
}

int shader_uniform_location_by_id(shader_ref ref, unsigned int id) {
	struct shader *shader = shaders+ref.id;
	return shader->uniform_locations[id];
}



int uniform_location(shader_ref ref, const char *name) {
	return glGetUniformLocation(gl_shader_object(ref), name);
}

void uniform3f(shader_ref ref, const char *name, float x, float y, float z) {
	glUniform3f(uniform_location(ref, name), x, y, z);
}

void uniform4f(shader_ref ref, const char *name, float x, float y, float z, float w) {
	glUniform4f(uniform_location(ref, name), x, y, z, w);
}

void uniform3fv(shader_ref ref, const char *name, float *v) {
	glUniform3fv(uniform_location(ref, name), 1, v);
}

void uniform4fv(shader_ref ref, const char *name, float *v) {
	glUniform4fv(uniform_location(ref, name), 1, v);
}

void uniform_matrix4x4f(shader_ref ref, const char *name, matrix4x4f *m) {
	glUniformMatrix4fv(uniform_location(ref, name), 1, GL_FALSE, m->col_major);
}




#ifdef WITH_GUILE
#include <libguile.h>

SCM_DEFINE(s_make_shader, "make-shader", 3, 0, 0, (SCM name, SCM input_n, SCM uniform_n), "create shader with name and number of input vars and uniforms.") {
	char *na = scm_to_locale_string(name);
	int ni = scm_to_int(input_n);
	int nu = scm_to_int(uniform_n);
	shader_ref ref = make_shader(na, ni, nu);
	free(na);
	return scm_from_int(ref.id);
}
SCM_DEFINE(s_destroy_shader, "destroy-shader", 1, 0, 0, (SCM shader), "") {
	shader_ref ref = { scm_to_int(shader) };
	bool valid = valid_shader_ref(ref);
	destroy_shader(ref);
	return valid ? SCM_BOOL_T : SCM_BOOL_F;
}
SCM_DEFINE(s_add_vertex_source, "add-vertex-source", 2, 0, 0, (SCM shader, SCM src), "add vertex shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_vertex_source(ref, source);
	free(source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_fragment_source, "add-fragment-source", 2, 0, 0, (SCM shader, SCM src), "add fragment shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_fragment_source(ref, source);
	free(source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_geometry_source, "add-geometry-source", 2, 0, 0, (SCM shader, SCM src), "add geometry shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_geometry_source(ref, source);
	free(source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_shader_input, "add-shader-input", 3, 0, 0, (SCM shader, SCM varname, SCM index), "") {
	shader_ref ref = { scm_to_int(shader) };
	char *vn = scm_to_locale_string(varname);
	int idx = scm_to_int(index);
	bool ret = add_shader_input(ref, vn, idx);
	free(vn);
	return ret ? SCM_BOOL_T : SCM_BOOL_F;
}
SCM_DEFINE(s_add_shader_uniform, "add-shader-uniform", 2, 0, 0, (SCM shader, SCM varname), "") {
	shader_ref ref = { scm_to_int(shader) };
	char *vn = scm_to_locale_string(varname);
	bool ret = add_shader_uniform(ref, vn);
	free(vn);
	return ret ? SCM_BOOL_T : SCM_BOOL_F;
}
SCM_DEFINE(s_compile_and_link_shader, "compile-and-link-shader", 1, 0, 0, (SCM shader), "") {
	shader_ref ref = { scm_to_int(shader) };
	bool ret = compile_and_link_shader(ref);
	return ret ? SCM_BOOL_T : SCM_BOOL_F;
}
SCM_DEFINE(s_find_shader, "find-shader", 1, 0, 0, (SCM name), "") {
	char *n = scm_to_locale_string(name);
	shader_ref ref = find_shader(n);
	free(n);
	if (valid_shader_ref(ref))
		return scm_from_int(ref.id);
	return SCM_BOOL_F;
}
SCM_DEFINE(s_valid_shader_ref, "valid-shader-ref", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	if (valid_shader_ref(ref))
		return SCM_BOOL_T;
	return SCM_BOOL_F;
}
SCM_DEFINE(s_vertex_shader_info_log, "vertex-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = vertex_shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_fragment_shader_info_log, "fragment-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = fragment_shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_geometry_shader_info_log, "geometry-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = geometry_shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_shader_link_info_log, "shader-link-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_shader_name, "shader-name", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	return scm_from_locale_string(shader_name(ref));
}

void register_scheme_functions_for_shaders() {
#ifndef SCM_MAGIC_SNARFER
#include "shader.x"
#endif
}
#endif
