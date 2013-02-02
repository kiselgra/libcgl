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
	char **vertex_source,
	     **fragment_source,
		 **geometry_source,
	     **tesselation_control_source,
		 **tesselation_evaluation_source,
		 **compute_source;
	int vertex_sources, fragment_sources, geometry_sources, tesselation_control_sources, tesselation_evaluation_sources, compute_sources;
	GLuint vertex_program, fragment_program, geometry_program, tess_control_program, tess_eval_program, compute_program;
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
	char *vert_info_log, *frag_info_log, *geom_info_log, *tess_control_info_log, *tess_eval_info_log, *compute_info_log, *program_info_log;
};

#ifndef SCM_MAGIC_SNARFER
#include "mm.h"
define_mm(shader, shaders, shader_ref);
#include "shader.xx"
#endif

shader_ref make_shader(const char *name, int input_vars) {
	shader_ref ref = allocate_shader_ref();
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
	shader->uniforms = 0;
	shader->next_uniform_id = 0;
	shader->uniform_names = 0;
	shader->uniform_locations = 0;
	shader->vertex_source = shader->fragment_source = shader->geometry_source = shader->tesselation_control_source = shader->tesselation_evaluation_source = shader->compute_source = 0;
	shader->vertex_sources = shader->fragment_sources = shader->geometry_sources = shader->tesselation_control_sources = shader->tesselation_evaluation_sources = shader->compute_sources = 0;
	shader->vertex_program = shader->fragment_program = shader->geometry_program = shader->tess_control_program = shader->tess_eval_program = shader->shader_program = shader->compute_program = 0;
	return ref;
}

void destroy_shader(shader_ref ref) {
	if (!valid_shader_ref(ref)) return;
	struct shader *shader = shaders+ref.id;
	for (int i = 0; i < shader->next_input_var; ++i) {
		free(shader->input_var_names[i]);
	}
	for (int i = 0; i < shader->vertex_sources; ++i)                 free(shader->vertex_source[i]);
	for (int i = 0; i < shader->fragment_sources; ++i)               free(shader->fragment_source[i]);
	for (int i = 0; i < shader->geometry_sources; ++i)               free(shader->geometry_source[i]);
	for (int i = 0; i < shader->tesselation_control_sources; ++i)    free(shader->tesselation_control_source[i]);
	for (int i = 0; i < shader->tesselation_evaluation_sources; ++i) free(shader->tesselation_evaluation_source[i]);
	for (int i = 0; i < shader->compute_sources; ++i)                free(shader->compute_source[i]);
	free(shader->input_var_names);                shader->input_var_names = 0;
	free(shader->input_var_ids);                  shader->input_var_ids = 0;
	free(shader->name);                           shader->name = 0;
	free(shader->vertex_source);                  shader->vertex_source = 0;
	free(shader->fragment_source);                shader->fragment_source = 0;
	free(shader->geometry_source);                shader->geometry_source = 0;
	free(shader->tesselation_control_source);     shader->tesselation_control_source = 0;
	free(shader->tesselation_evaluation_source);  shader->tesselation_evaluation_source = 0;
	free(shader->compute_source);                 shader->compute_source = 0;
	if (shader->shader_program)       { glDeleteProgram(shader->shader_program);      shader->shader_program = 0; }
	if (shader->vertex_program)       { glDeleteShader(shader->vertex_program);       shader->vertex_program = 0; }
	if (shader->fragment_program)     { glDeleteShader(shader->fragment_program);     shader->fragment_program = 0; }
	if (shader->geometry_program)     { glDeleteShader(shader->geometry_program);     shader->geometry_program = 0; }
	if (shader->tess_control_program) { glDeleteShader(shader->tess_control_program); shader->tess_control_program = 0; }
	if (shader->tess_eval_program)    { glDeleteShader(shader->tess_eval_program);    shader->tess_eval_program = 0; }
	if (shader->compute_program)      { glDeleteShader(shader->compute_program);      shader->compute_program = 0; }
	free_shader_ref(ref);
}

void add_shader_source(char ***destination, const char *add, int *size) {
	// each shader source type holds an array of strings.
	unsigned int old_size = *size;
	char **new_array = malloc((old_size+1)*sizeof(char**));
	for (int i = 0; i < old_size; ++i)
		new_array[i] = (*destination)[i];
	free(*destination);
	new_array[old_size] = strdup(add);
	(*size)++;
	*destination = new_array;
}

void add_vertex_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->vertex_source, src, &shader->vertex_sources);
}

void add_fragment_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->fragment_source, src, &shader->fragment_sources);
}

void add_geometry_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->geometry_source, src, &shader->geometry_sources);
}

void add_tesselation_control_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->tesselation_control_source, src, &shader->tesselation_control_sources);
}

void add_tesselation_evaluation_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->tesselation_evaluation_source, src, &shader->tesselation_evaluation_sources);
}

void add_compute_source(shader_ref ref, const char *src) {
	struct shader *shader = shaders+ref.id;
	add_shader_source(&shader->compute_source, src, &shader->compute_sources);
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

static void establish_uniform_location(struct shader *shader, int i) {
	int loc = glGetUniformLocation(shader->shader_program, shader->uniform_names[i]);
	if (loc < 0)
		fprintf(stderr, "WARNING: Location of uniform %s in shader %s is < 0.\n", shader->uniform_names[i], shader->name);
	shader->uniform_locations[i] = loc;
}

bool add_shader_uniform(shader_ref ref, const char *name) {
	struct shader *shader = shaders+ref.id;
	if (shader->next_uniform_id >= shader->uniforms) {
// 		fprintf(stderr, "tried to assign shader uniform (%s, %d) to shader %s when all bindings are set.\n", name, shader->next_uniform_id, shader->name);
// 		return false;
		shader->uniform_names = realloc(shader->uniform_names, sizeof(char*) * shader->uniforms+1);
		shader->uniform_locations = realloc(shader->uniform_locations, sizeof(int) * shader->uniforms+1);
		shader->uniforms++;

	}
	int i = shader->next_uniform_id++;
	shader->uniform_names[i] = malloc(strlen(name)+1);
	strcpy(shader->uniform_names[i], name);
	if (shader->built_ok)
		establish_uniform_location(shader, i);
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
	GLint compile_res;

	// compile shader source
	if (shader->vertex_source) {
		shader->vertex_program = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shader->vertex_program, shader->vertex_sources, (const char**)shader->vertex_source, 0);
		glCompileShader(shader->vertex_program);
	}

	if (shader->fragment_source) {
		shader->fragment_program = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shader->fragment_program, shader->fragment_sources, (const char**)shader->fragment_source, 0);
		glCompileShader(shader->fragment_program);
	}
	
#if CGL_GL == GL
#if CGL_GL_VERSION >= GL3
	if (shader->geometry_source) {
		shader->geometry_program = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shader->geometry_program, shader->geometry_sources, (const char**)shader->geometry_source, 0);
		glCompileShader(shader->geometry_program);
	}
#endif
#if CGL_GL_VERSION >= GL4
	if (shader->tesselation_control_source) {
		shader->tess_control_program = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(shader->tess_control_program, shader->tesselation_control_sources, (const char**)shader->tesselation_control_source, 0);
		glCompileShader(shader->tess_control_program);
	}
	if (shader->tesselation_evaluation_source) {
		shader->tess_eval_program = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(shader->tess_eval_program, shader->tesselation_evaluation_sources, (const char**)shader->tesselation_evaluation_source, 0);
		glCompileShader(shader->tess_eval_program);
	}
	if (shader->compute_source) {
		shader->compute_program = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader->compute_program, shader->compute_sources, (const char**)shader->compute_source, 0);
		glCompileShader(shader->compute_program);
	}
#endif
#endif

	if (shader->vertex_source) {
		glGetShaderiv(shader->vertex_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->vert_info_log, shader->vertex_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			if (shader->geometry_source) glDeleteShader(shader->geometry_program);
			fprintf(stderr, "failed to compile vertex shader of %s\n", shader->name);
			return false;
		}
	}

	if (shader->fragment_source) {
		glGetShaderiv(shader->fragment_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->frag_info_log, shader->fragment_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			if (shader->geometry_source) glDeleteShader(shader->geometry_program);
			if (shader->tess_control_program) glDeleteShader(shader->tess_control_program);
			if (shader->tess_eval_program) glDeleteShader(shader->tess_eval_program);
			fprintf(stderr, "failed to compile fragment shader of %s\n", shader->name);
			return false;
		}
	}

#if CGL_GL == GL
#if CGL_GL_VERSION >= GL3
	if (shader->geometry_source) {
		glGetShaderiv(shader->geometry_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->geom_info_log, shader->geometry_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			glDeleteShader(shader->geometry_program);
			if (shader->tess_control_program) glDeleteShader(shader->tess_control_program);
			if (shader->tess_eval_program) glDeleteShader(shader->tess_eval_program);
			fprintf(stderr, "failed to compile geometry shader of %s\n", shader->name);
			return false;
		}
	}
#endif
#if CGL_GL_VERSION >= GL4
	if (shader->tesselation_control_source) {
		glGetShaderiv(shader->tess_control_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->tess_control_info_log, shader->tess_control_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			glDeleteShader(shader->geometry_program);
			glDeleteShader(shader->tess_control_program);
			if (shader->tess_eval_program) glDeleteShader(shader->tess_eval_program);
			fprintf(stderr, "failed to compile tesselation control shader of %s\n", shader->name);
			return false;
		}
	}
	if (shader->tesselation_evaluation_source) {
		glGetShaderiv(shader->tess_eval_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->tess_eval_info_log, shader->tess_eval_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			glDeleteShader(shader->geometry_program);
			glDeleteShader(shader->tess_control_program);
			glDeleteShader(shader->tess_eval_program);
			fprintf(stderr, "failed to compile tesselation evaluation shader of %s\n", shader->name);
			return false;
		}
	}
	if (shader->compute_source) {
		glGetShaderiv(shader->compute_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->compute_info_log, shader->compute_program);
			glDeleteShader(shader->vertex_program);
			glDeleteShader(shader->fragment_program);
			glDeleteShader(shader->geometry_program);
			glDeleteShader(shader->tess_control_program);
			glDeleteShader(shader->tess_eval_program);
			glDeleteShader(shader->compute_program);
			fprintf(stderr, "failed to compile compute shader of %s\n", shader->name);
			return false;
		}
	}
#endif
#endif

	shader->shader_program = glCreateProgram();
	if (shader->vertex_program) glAttachShader(shader->shader_program, shader->vertex_program);
	if (shader->fragment_program) glAttachShader(shader->shader_program, shader->fragment_program);
#if CGL_GL == GL
#if CGL_GL_VERSION >= GL3
	if (shader->geometry_program) glAttachShader(shader->shader_program, shader->geometry_program);
#endif
#if CGL_GL_VERSION >= GL4
	if (shader->tess_control_program) glAttachShader(shader->shader_program, shader->tess_control_program);
	if (shader->tess_eval_program) glAttachShader(shader->shader_program, shader->tess_eval_program);
	if (shader->compute_program) glAttachShader(shader->shader_program, shader->compute_program);
#endif
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

	for (int i = 0; i < shader->uniforms; ++i)
		establish_uniform_location(shader, i);

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

const char *tess_control_shader_info_log(shader_ref ref) {
	return shaders[ref.id].tess_control_info_log;
}

const char *tess_eval_shader_info_log(shader_ref ref) {
	return shaders[ref.id].tess_eval_info_log;
}

const char *compute_shader_info_log(shader_ref ref) {
	return shaders[ref.id].compute_info_log;
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
	for (int i = 0; i < next_shader_index; ++i) {
		if (strcmp(shaders[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	}
	return ref;
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

SCM_DEFINE(s_make_shader, "make-shader", 2, 0, 0, (SCM name, SCM input_n), "create shader with name and number of input vars and uniforms.") {
	char *na = scm_to_locale_string(name);
	int ni = scm_to_int(input_n);
	shader_ref ref = make_shader(na, ni);
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
SCM_DEFINE(s_add_tc_source, "add-tesselation-control-source", 2, 0, 0, (SCM shader, SCM src), "add tesselation control shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_tesselation_control_source(ref, source);
	free(source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_te_source, "add-tesselation-evaluation-source", 2, 0, 0, (SCM shader, SCM src), "add tesselation evaluation shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_tesselation_evaluation_source(ref, source);
	free(source);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_add_comp_source, "add-compute-source", 2, 0, 0, (SCM shader, SCM src), "add compute shader source to the shader object.") {
	shader_ref ref = { scm_to_int(shader) };
	char *source = scm_to_locale_string(src);
	add_compute_source(ref, source);
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
SCM_DEFINE(s_tc_shader_info_log, "tesselation-control-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = tess_control_shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_te_shader_info_log, "tesselation-evaluation-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = tess_eval_shader_info_log(ref);
	return scm_from_locale_string(log ? log : (const char*)"nil");
}
SCM_DEFINE(s_comp_shader_info_log, "compute-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = compute_shader_info_log(ref);
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
SCM_DEFINE(s_bind_shader, "bind-shader", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	bind_shader(ref);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_unbind_shader, "unbind-shader", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	unbind_shader(ref);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_shader_uniform_loc, "uniform-location", 2, 0, 0, (SCM id, SCM uni), "") {
	char *n = scm_to_locale_string(uni);
	shader_ref ref = { scm_to_int(id) };
	int loc = uniform_location(ref, n);
	free(n);
	return scm_from_int(loc);
}

void register_scheme_functions_for_shaders() {
#ifndef SCM_MAGIC_SNARFER
#include "shader.x"
#endif
}
#endif
