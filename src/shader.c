#define _GNU_SOURCE

#include "cgl.h"
#include "shader.h"
#include "mesh.h"		// just for shader-error texture
#include "texture.h"	// just for shader-error texture
#include "prepared.h"	// just for shader-error texture
#include "impex.h"		// just for shader-error texture
#include "gl-version.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*! \defgroup shaders Shaders
 * 	See \ref shader.h for a detailled description.
 *
 *  \section shaderusage General usage.
 *
 *  To create a shader call \ref make_shader and give it a unique name.
 *  The code is large, but rather simple.
 *	
 *  A few uniform-handling functions are defined at the end of the C part of this file.
 *
 *
 *  \section shaderreload A note on consistent shader reloading.
 *
 *  When a shader is deleted it's index-slot is free to take by another shader.
 *  As long as the list of free indices is not empty the next shader will be
 *  allocated the index put last on the list.
 *
 *  This means it is safe to assume a shader keeps its id when reload is
 *  implemented as follows:
 *  \li  shader_ref old_ref = find_shader("my-shader");
 *  \li  destroy_shader(old_ref);
 *  \li  shader_ref new_ref = make_shader("my-shader", ...);
 *  \li  --> old_ref.id == new_ref.id
 *
 *
 *  \section shaderreload2 Automatic shader file reload
 *
 * 	A shader reaload can be triggered from scheme by calling \c (trigger-reload-of-shader-files)
 * 	(or an alias defined in scheme/shader.scm). This just sets the flag \c cgl_shader_reload_pending
 * 	which has to be checked in the applications mainloop. If it is set the application should call
 * 	\c reload_shaders which will call the scheme function \c (execute-shader-reload). Note that this
 * 	function cannot just be called from scheme as its calling OpenGL functions which can only be
 * 	called from the thread holding an OpenGL context. That's the reason for the whole indirection
 * 	process.
 *
 * 	\note For shaders to be known to the reloading process they have to loaded using
 * 	\c load-shader-file, initiallay.
 *
 */

/*! \file shader.h
 *  \ingroup shaders
 *
 * 	Why can't I copy the group documentation here?
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

/*! \addtogroup shaders
 *  @{
 */

shader_ref make_shader(const char *name, int input_vars) {
// 	printf("---> make shader %s.\n", name);
	shader_ref ref = allocate_shader_ref();
	struct shader *shader = shaders+ref.id;
	shader->name = malloc(strlen(name)+1);
	strcpy(shader->name, name);
	shader->bound = false;
	shader->built_ok = false;
	shader->input_vars = input_vars;
	shader->next_input_var = 0;
	shader->vert_info_log = shader->frag_info_log = shader->geom_info_log = shader->tess_control_info_log = shader->tess_eval_info_log = shader->compute_info_log = shader->program_info_log = 0;
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
// 	printf("---> destroy shader %s.\n", shader->name);
	for (int i = 0; i < shader->next_input_var; ++i) {
		free(shader->input_var_names[i]);
	}
	for (int i = 0; i < shader->vertex_sources; ++i)                 free(shader->vertex_source[i]);
	for (int i = 0; i < shader->fragment_sources; ++i)               free(shader->fragment_source[i]);
	for (int i = 0; i < shader->geometry_sources; ++i)               free(shader->geometry_source[i]);
	for (int i = 0; i < shader->tesselation_control_sources; ++i)    free(shader->tesselation_control_source[i]);
	for (int i = 0; i < shader->tesselation_evaluation_sources; ++i) free(shader->tesselation_evaluation_source[i]);
	for (int i = 0; i < shader->compute_sources; ++i)                free(shader->compute_source[i]);
	if (shader->shader_program)       { glDeleteProgram(shader->shader_program);      shader->shader_program = 0; }
	if (shader->vertex_program)       { glDeleteShader(shader->vertex_program);       shader->vertex_program = 0; }
	if (shader->fragment_program)     { glDeleteShader(shader->fragment_program);     shader->fragment_program = 0; }
	if (shader->geometry_program)     { glDeleteShader(shader->geometry_program);     shader->geometry_program = 0; }
	if (shader->tess_control_program) { glDeleteShader(shader->tess_control_program); shader->tess_control_program = 0; }
	if (shader->tess_eval_program)    { glDeleteShader(shader->tess_eval_program);    shader->tess_eval_program = 0; }
	if (shader->compute_program)      { glDeleteShader(shader->compute_program);      shader->compute_program = 0; }
	free(shader->input_var_names);                shader->input_var_names = 0;
	free(shader->input_var_ids);                  shader->input_var_ids = 0;
	free(shader->vertex_source);                  shader->vertex_source = 0;
	free(shader->fragment_source);                shader->fragment_source = 0;
	free(shader->geometry_source);                shader->geometry_source = 0;
	free(shader->tesselation_control_source);     shader->tesselation_control_source = 0;
	free(shader->tesselation_evaluation_source);  shader->tesselation_evaluation_source = 0;
	free(shader->compute_source);                 shader->compute_source = 0;
	free(shader->name);                           shader->name = 0;
	free_shader_ref(ref);
}

//! @}

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

/*! \addtogroup shaders
 *  @{
 */

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

//! @}

static void establish_uniform_location(struct shader *shader, int i) {
	int loc = glGetUniformLocation(shader->shader_program, shader->uniform_names[i]);
	if (loc < 0)
		fprintf(stderr, "WARNING: Location of uniform %s in shader %s is < 0.\n", shader->uniform_names[i], shader->name);
	shader->uniform_locations[i] = loc;
}

/*! \addtogroup shaders
 *  @{
 */

bool add_shader_uniform(shader_ref ref, const char *name) {
	struct shader *shader = shaders+ref.id;
	if (shader->next_uniform_id >= shader->uniforms) {
// 		fprintf(stderr, "tried to assign shader uniform (%s, %d) to shader %s when all bindings are set.\n", name, shader->next_uniform_id, shader->name);
// 		return false;
		shader->uniform_names = realloc(shader->uniform_names, sizeof(char*) * (shader->uniforms+1));
		shader->uniform_locations = realloc(shader->uniform_locations, sizeof(int) * (shader->uniforms+1));
		shader->uniforms++;

	}
	int i = shader->next_uniform_id++;
	shader->uniform_names[i] = malloc(strlen(name)+1);
	strcpy(shader->uniform_names[i], name);
	if (shader->built_ok)
		establish_uniform_location(shader, i);
	return true;
}

//! @}

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
// 		printf("len = %d, written = %d\n", len, written);
		if (glIsShader(object))       glGetShaderInfoLog(object, len, &written, *target);
		else if (glIsProgram(object)) glGetProgramInfoLog(object, len, &written, *target);
		else                          strcpy(*target, "tried to get info log of an object which is neither a shader nor a program"), written = len;
	}
	(*target)[len] = '\0';
}

/*! \addtogroup shaders
 *  @{
 */

bool compile_and_link_shader(shader_ref ref) {
	struct shader *shader = shaders+ref.id;
	GLint compile_res;

	shader->built_ok = false;

	check_for_gl_errors("before compile");
	// compile shader source
	if (shader->vertex_source) {
		shader->vertex_program = glCreateShader(GL_VERTEX_SHADER);
// 		printf("C&L shader->vertex_program = %d.\n", shader->vertex_program);
		glShaderSource(shader->vertex_program, shader->vertex_sources, (const char**)shader->vertex_source, 0);
		glCompileShader(shader->vertex_program);
	}

	if (shader->fragment_source) {
		shader->fragment_program = glCreateShader(GL_FRAGMENT_SHADER);
// 		printf("C&L shader->fragment_program = %d.\n", shader->fragment_program);
		glShaderSource(shader->fragment_program, shader->fragment_sources, (const char**)shader->fragment_source, 0);
		glCompileShader(shader->fragment_program);
	}
	
#if CGL_GL == GL
#if CGL_GL_VERSION >= GL3
	if (shader->geometry_source) {
		shader->geometry_program = glCreateShader(GL_GEOMETRY_SHADER);
// 		printf("C&L shader->geometry_program = %d.\n", shader->geometry_program);
		glShaderSource(shader->geometry_program, shader->geometry_sources, (const char**)shader->geometry_source, 0);
		glCompileShader(shader->geometry_program);
	}
#endif
#if CGL_GL_VERSION >= GL4
	if (shader->tesselation_control_source) {
		shader->tess_control_program = glCreateShader(GL_TESS_CONTROL_SHADER);
// 		printf("C&L shader->tesselation_control_program = %d.\n", shader->tess_control_program);
		glShaderSource(shader->tess_control_program, shader->tesselation_control_sources, (const char**)shader->tesselation_control_source, 0);
		glCompileShader(shader->tess_control_program);
	}
	if (shader->tesselation_evaluation_source) {
		shader->tess_eval_program = glCreateShader(GL_TESS_EVALUATION_SHADER);
// 		printf("C&L shader->tesselation_evaluation_program = %d.\n", shader->tess_eval_program);
		glShaderSource(shader->tess_eval_program, shader->tesselation_evaluation_sources, (const char**)shader->tesselation_evaluation_source, 0);
		glCompileShader(shader->tess_eval_program);
	}
	if (shader->compute_source) {
		shader->compute_program = glCreateShader(GL_COMPUTE_SHADER);
// 		printf("C&L shader->compute_program = %d.\n", shader->compute_program);
		glShaderSource(shader->compute_program, shader->compute_sources, (const char**)shader->compute_source, 0);
		glCompileShader(shader->compute_program);
	}
#endif
#endif

	if (shader->vertex_source) {
		glGetShaderiv(shader->vertex_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->vert_info_log, shader->vertex_program);
			glDeleteShader(shader->vertex_program);     shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);   shader->fragment_program = 0;
			if (shader->geometry_source) { glDeleteShader(shader->geometry_program);   shader->geometry_program = 0; }
			fprintf(stderr, "failed to compile vertex shader of %s\n", shader->name);
			return false;
		}
	}

	if (shader->fragment_source) {
		glGetShaderiv(shader->fragment_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->frag_info_log, shader->fragment_program);
			glDeleteShader(shader->vertex_program);    shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);  shader->fragment_program = 0;
			if (shader->geometry_source)      { glDeleteShader(shader->geometry_program);      shader->geometry_program = 0; }
			if (shader->tess_control_program) { glDeleteShader(shader->tess_control_program);  shader->tess_control_program = 0; }
			if (shader->tess_eval_program)    { glDeleteShader(shader->tess_eval_program);     shader->tess_eval_program = 0; }
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
			glDeleteShader(shader->vertex_program);     shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);   shader->fragment_program = 0;
			glDeleteShader(shader->geometry_program);   shader->geometry_program = 0;
			if (shader->tess_control_program) { glDeleteShader(shader->tess_control_program);  shader->tess_control_program = 0; }
			if (shader->tess_eval_program)    { glDeleteShader(shader->tess_eval_program);     shader->tess_eval_program = 0; }
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
			glDeleteShader(shader->vertex_program);        shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);      shader->fragment_program = 0;
			glDeleteShader(shader->geometry_program);      shader->geometry_program = 0;
			glDeleteShader(shader->tess_control_program);  shader->tess_control_program = 0;
			if (shader->tess_eval_program) { glDeleteShader(shader->tess_eval_program);   shader->tess_eval_program = 0; }
			fprintf(stderr, "failed to compile tesselation control shader of %s\n", shader->name);
			return false;
		}
	}
	if (shader->tesselation_evaluation_source) {
		glGetShaderiv(shader->tess_eval_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->tess_eval_info_log, shader->tess_eval_program);
			glDeleteShader(shader->vertex_program);        shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);      shader->fragment_program = 0;
			glDeleteShader(shader->geometry_program);      shader->geometry_program = 0;
			glDeleteShader(shader->tess_control_program);  shader->tess_control_program = 0;
			glDeleteShader(shader->tess_eval_program);     shader->tess_eval_program = 0;
			fprintf(stderr, "failed to compile tesselation evaluation shader of %s\n", shader->name);
			return false;
		}
	}
	if (shader->compute_source) {
		glGetShaderiv(shader->compute_program, GL_COMPILE_STATUS, &compile_res);
		if (compile_res == GL_FALSE) {
			store_info_log(&shader->compute_info_log,       shader->compute_program);
			glDeleteShader(shader->vertex_program);         shader->vertex_program = 0;
			glDeleteShader(shader->fragment_program);       shader->fragment_program = 0;
			glDeleteShader(shader->geometry_program);       shader->geometry_program = 0;
			glDeleteShader(shader->tess_control_program);   shader->tess_control_program = 0;
			glDeleteShader(shader->tess_eval_program);      shader->tess_eval_program = 0;
			glDeleteShader(shader->compute_program);        shader->compute_program = 0;
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

 	// TODO cleanup
// 	glProgramParameteri(shader->shader_program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	// TODO end
 
	// link
	glLinkProgram(shader->shader_program);
	glGetProgramiv(shader->shader_program, GL_LINK_STATUS, &compile_res);
	if (compile_res == GL_FALSE) {
		store_info_log(&shader->program_info_log, shader->shader_program);
		fprintf(stderr, "faild to link shader %s\n", shader->name);
		return false;
	}
	
	shader->built_ok = true;
        
 	// TODO cleanup
// 		int binary_length;
//         glGetProgramiv(shader->shader_program, GL_PROGRAM_BINARY_LENGTH, &binary_length);
// 		printf("length of binary of shader %s is %d.\n", shader->name, binary_length);
// 
// 		char *binary = malloc(binary_length+1);
//         GLenum binary_format;
//         glGetProgramBinary(shader->shader_program, binary_length, 0, &binary_format, binary);
// 
// 			printf("----------- binary for shader %s --------------\n", shader->name);
// 			for (int i = 0; i < binary_length; ++i)
// 				putchar(binary[i]);
// 			printf("\n----------- end of binary for shader %s --------------\n", shader->name);
// 			exit(-1);
	// TODO end



	for (int i = 0; i < shader->uniforms; ++i)
		establish_uniform_location(shader, i);

	check_for_gl_errors("end of compile");
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

//! usable means: valid ref and built fine.
bool shader_usable(shader_ref ref) {
	if (valid_shader_ref(ref))
		return shaders[ref.id].built_ok;
	return false;
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

const char** shader_uniform_names(shader_ref ref) {
	return (const char **)shaders[ref.id].uniform_names;
}

const int* shader_uniform_locations(shader_ref ref) {
	return shaders[ref.id].uniform_locations;
}

int shader_uniform_location_by_id(shader_ref ref, unsigned int id) {
	struct shader *shader = shaders+ref.id;
	return shader->uniform_locations[id];
}

vec3i compute_shader_local_workgroup_size(shader_ref ref) {
	GLuint so = gl_shader_object(ref);
	vec3i v;
	glGetProgramiv(so, GL_COMPUTE_WORK_GROUP_SIZE, (GLint*)&v);
	return v;
}

//! dispatches a compute of <em>at least</em> size_x*size_y*size_z (rounded up to multiples of the defined work sizes, each).
void dispatch_compute(shader_ref ref, int size_x, int size_y, int size_z) {
	vec3i local_size = compute_shader_local_workgroup_size(ref);
	float x = ((float)size_x)/local_size.x,
		  y = ((float)size_y)/local_size.y,
		  z = ((float)size_z)/local_size.z;
	int w = (int)x; if (floor(x) != x) ++w;
	int h = (int)y; if (floor(y) != y) ++h;
	int d = (int)z; if (floor(z) != z) ++d;
	glDispatchCompute(w, h, d);
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

//! @}

bool cgl_shader_reload_pending = false;

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
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_fragment_shader_info_log, "fragment-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = fragment_shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_geometry_shader_info_log, "geometry-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = geometry_shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_tc_shader_info_log, "tesselation-control-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = tess_control_shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_te_shader_info_log, "tesselation-evaluation-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = tess_eval_shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_comp_shader_info_log, "compute-shader-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = compute_shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
}
SCM_DEFINE(s_shader_link_info_log, "shader-link-info-log", 1, 0, 0, (SCM id), "") {
	shader_ref ref = { scm_to_int(id) };
	const char *log = shader_info_log(ref);
	if (log) return scm_from_locale_string(log);
	return SCM_EOL;
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

SCM_DEFINE(s_trigger_shader_reload, "trigger-reload-of-shader-files", 0, 0, 0, (), "") {
	cgl_shader_reload_pending = true;
	return SCM_BOOL_T;
}

SCM_DEFINE(s_check_gl_error, "check-for-gl-error", 1, 0, 0, (SCM s), "") {
	char *str = scm_to_locale_string(s);
	check_for_gl_errors(str);
	free(str);
	return SCM_BOOL_T;
}

void reload_shaders() {
	scm_c_eval_string("(execute-shader-reload \"\")");
	cgl_shader_reload_pending = false;
}

void reload_shader(const char *filename) {
	char *tmp = 0;
	int n = asprintf(&tmp, "(execute-shader-reload \"%s\")", filename);
	scm_c_eval_string(tmp);
	free(tmp);
}

static mesh_ref shader_error_quad = { -1 };
static texture_ref shader_error_tex = { -1 };
static shader_ref shader_error_shader = { -1 };
static char *shader_error_message = 0;

void make_shader_error_display(int w, int h) {
#if LIBCGL_HAVE_LIBCAIRO == 1
	shader_error_quad = make_quad_with_tc("shader error display", 0);
	tex_params_t p = default_fbo_tex_params();
	shader_error_tex =  make_empty_texture("shader error texture", w, h, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
	shader_error_shader = find_shader("cgl/shader-error-shader");
#else
	fprintf(stderr, "Cairo support was not compiled in, this will not produce output!\n");
#endif
}

bool shader_errors_present() {
	return scm_is_true(scm_c_eval_string("shader-errors"));
}

#if LIBCGL_HAVE_LIBCAIRO == 1

#include <cairo/cairo.h>

static cairo_t *cairo = 0;
static cairo_surface_t *cairo_surface = 0;
static unsigned char *cairo_surface_data = 0;

//! literally taken from cgls/console.c.in
static cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer) {
	cairo_t *cr;

	/* create cairo-surface/context to act as OpenGL-texture source */
	*buffer = (unsigned char*)calloc(channels * width * height, sizeof(unsigned char));
	if (!*buffer) {
		printf("create_cairo_context() - Couldn't allocate surface-buffer\n");
		return 0;
	}

	*surf = cairo_image_surface_create_for_data(*buffer, CAIRO_FORMAT_ARGB32, width, height, channels * width);
	if (cairo_surface_status(*surf) != CAIRO_STATUS_SUCCESS) {
		free(*buffer);
		printf("create_cairo_context() - Couldn't create surface\n");
		return 0;
	}

	cr = cairo_create(*surf);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		free(*buffer);
		printf("create_cairo_context() - Couldn't create context\n");
		return 0;
	}
	
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	return cr;
}

void regenerate_error_texture(char *text) {
	int max_line_len = 256;
	char *line = malloc(max_line_len+1);
	int w = texture_width(shader_error_tex);
	int h = texture_height(shader_error_tex);
	if (!cairo)
		cairo = create_cairo_context(w, h, 4, &cairo_surface, &cairo_surface_data);

	cairo_set_source_rgb(cairo, 0, 0, 0);

	cairo_set_font_size(cairo, 12);
	char *font_name = scm_to_locale_string(scm_c_eval_string("shader-error-font-name"));
	cairo_select_font_face(cairo, font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	free(font_name);
	cairo_font_extents_t fe;
	cairo_font_extents(cairo, &fe);

	int len = strlen(text);
	int start_i = 0;
	int line_nr = 0;
	int xoffset = 20,
		yoffset = 20;
	int max_line_width = 0;
	for (int i = 0; i < len; ++i) {
		if (text[i] == '\n') {
			int line_len = i - start_i;
			if (line_len > max_line_len) {
				free(line);
				line = malloc(line_len+1);
			}
			strncpy(line, text+start_i, line_len);
			line[line_len] = '\0';
			cairo_move_to(cairo, xoffset, yoffset);
			cairo_show_text(cairo, line);
			yoffset += fe.height;
			cairo_text_extents_t te;
			cairo_text_extents(cairo, line, &te);
			if (te.width > max_line_width)
				max_line_width = te.width;
			if (yoffset > h - fe.height - 20) {
				if (max_line_width > w/2-20 || xoffset > w/2) {
					cairo_move_to(cairo, w-40, h-10);
					cairo_set_source_rgb(cairo, 0, 0, .7);
					cairo_show_text(cairo, "...");
					cairo_set_source_rgb(cairo, 0, 0, 0);
					break;
				}
				yoffset = 20;
				xoffset = 20+w/2;
			}
			start_i = i+1;
			++line_nr;
		}
	}
	
	bind_texture(shader_error_tex, 0);
	unsigned char *data = cairo_image_surface_get_data(cairo_surface);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	unbind_texture(shader_error_tex);

// 	save_texture_as_png(shader_error_tex, "tex.png");
}

#endif

void render_shader_error_message() {
#if LIBCGL_HAVE_LIBCAIRO == 1
	char *error_text = scm_to_locale_string(scm_c_eval_string("shader-error-texts"));
	if (!shader_error_message || strcmp(shader_error_message, error_text) != 0) {
		regenerate_error_texture(error_text);
		free(shader_error_message);
		shader_error_message = error_text;
	}
	else
		free(error_text);

// 	texture_ref tex = find_texture("gbuffer/normal");
	texture_ref tex = find_texture("shader error texture");
	glDisable(GL_DEPTH_TEST);
	bind_shader(shader_error_shader);
	bind_mesh_to_gl(shader_error_quad);
	bind_texture(tex, 0);
	draw_mesh(shader_error_quad);
	unbind_texture(tex);
	unbind_mesh_from_gl(shader_error_quad);
	unbind_shader(shader_error_shader);
	glEnable(GL_DEPTH_TEST);
#endif	
}

#include <sys/inotify.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define max_inotify_watches 512
static int inotify_fd = -1;
struct pollfd poll_fds[1];
static int inotify_watches[max_inotify_watches];
static char *inotify_watched_files[max_inotify_watches];
static int inotify_watches_N = 0;

static bool inotify_verbose = false;

void activate_automatic_shader_reload() {
	fprintf(stderr, "ino\n");
	if (inotify_fd == -1) {
		inotify_fd = inotify_init1(IN_NONBLOCK);
		poll_fds[0].fd = inotify_fd;
		poll_fds[0].events = POLLIN;
	}

	SCM file_list = scm_c_eval_string("shader-files");
	while (!scm_is_null(file_list)) {
		SCM car = scm_car(file_list);
		file_list = scm_cdr(file_list);
		char *file = scm_to_locale_string(car);
		if (inotify_watches_N >= max_inotify_watches) {
			fprintf(stderr, "Adding a watch on shader file '%s': too many inotify watches. Can be extended, see %s:%d.\n", file, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
		inotify_watches[inotify_watches_N] = inotify_add_watch(inotify_fd, file, inotify_verbose ? IN_ALL_EVENTS : (IN_MODIFY | IN_IGNORED));
		inotify_watched_files[inotify_watches_N] = file;	// that's why we don't free it.
		if (inotify_watches[inotify_watches_N] == -1) {
			fprintf(stderr, "Cannot watch '%s'\n", file);
			perror("inotify_add_watch");
			exit(EXIT_FAILURE);
		}
		inotify_watches_N++;
	}
}

/* right now we get a SELF_DELETE followed by an IGNORE which meas the watch is dropped. something with how vi saves the file? */

static void handle_inotify_events() {
	/* Some systems cannot read integer variables if they are not
	   properly aligned. On other systems, incorrect alignment may
	   decrease performance. Hence, the buffer used for reading from
	   the inotify file descriptor should have the same alignment as
	   struct inotify_event. */

	static char buf[4096]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int i;
	ssize_t len;
	char *ptr;

	// Loop while events can be read from inotify file descriptor.
	while (1) {
		// Read events
		len = read(inotify_fd, buf, sizeof buf);
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		// If the nonblocking read() found no events to read, then it returns
 		// -1 with errno set to EAGAIN. In that case, we exit the loop. 
		if (len <= 0)
			break;

		// Loop over all events in the buffer
		for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
			event = (const struct inotify_event *) ptr;
			char *file = 0;
			int index = 0;
			for (index = 0; index < inotify_watches_N; ++index)
				if (inotify_watches[index] == event->wd) {
					file = inotify_watched_files[index];
					break;
				}
			bool exists = file_exists(file);
			if (inotify_verbose) {
				printf("FILE:  %s\n", file);
				printf("EVENT: %d %d %d %d %s\n", event->wd, event->mask, event->cookie, event->len, event->name);
				if (exists) printf("EXIST: yes.\n");
				else        printf("EXIST: no.\n");

				if (event->mask & IN_ACCESS) printf("M: IN_ACCESS\n");
				if (event->mask & IN_ATTRIB) printf("M: IN_ATTRIB\n");
				if (event->mask & IN_CLOSE_WRITE) printf("M: IN_CLOSE_WRITE\n");
				if (event->mask & IN_CLOSE_NOWRITE) printf("M: IN_CLOSE_NOWRITE\n");
				if (event->mask & IN_CREATE) printf("M: IN_CREATE\n");
				if (event->mask & IN_DELETE) printf("M: IN_DELETE\n");
				if (event->mask & IN_DELETE_SELF) printf("M: IN_DELETE_SELF\n");
				if (event->mask & IN_MODIFY) printf("M: IN_MODIFY\n");
				if (event->mask & IN_MOVE_SELF) printf("M: IN_MOVE_SELF\n");
				if (event->mask & IN_MOVED_FROM) printf("M: IN_MOVED_FROM\n");
				if (event->mask & IN_MOVED_TO) printf("M: IN_MOVED_TO\n");
				if (event->mask & IN_OPEN) printf("M: IN_OPEN\n");
			}
			bool reload = false;
			if (event->mask & IN_MODIFY)
				reload = true;
			else if (event->mask & IN_IGNORED)
				if (exists) {
					reload = true;
					inotify_watches[index] = inotify_add_watch(inotify_fd, file, inotify_verbose ? IN_ALL_EVENTS : (IN_MODIFY | IN_IGNORED));
				}
				else {
					fprintf(stderr, "Missing logic in shader reload: File was (presumably) changed by copying over it (like vim :w does), but the file no longer exists. Maybe it is really gone. Maybe we should poll for a few frames.\n");
					exit(EXIT_FAILURE);
				}
			if (reload) {
				printf("reloading %s\n", file);
				reload_shader(file);
			}
		}
	}
}

void reload_modified_shader_files() {
	int poll_num = poll(poll_fds, 1, 0);
	if (poll_num == -1) {
		if (errno == EINTR)
			return;
		perror("poll for reload_modified_shader_files");
		exit(EXIT_FAILURE);
	}
	if (poll_num > 0) {
		printf("polled something!\n");
		handle_inotify_events();
	}
}


void register_scheme_functions_for_shaders() {
#ifndef SCM_MAGIC_SNARFER
#include "shader.x"
#endif
}
#endif
