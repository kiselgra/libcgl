#include "mesh.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

/*! \defgroup meshes Meshes
 *
 *  A mesh is a very simple thing.
 *  It is just a collection of vertex attributes (i.e. positions, normals,
 *  texture coordinates) used to support data for a draw call.
 *  It does not hold any references to textures, colors, materials, shaders or
 *  whatever.
 *
 *  You can add a vertex buffer to a mesh in two different ways.
 *  First: add it by passing cpu data -> a vbo will be created.
 *  Second: add an existing vbo -> it will be used. (create the mesh with
 *  make_mesh("", 0)).
 *  \attention don't mix the two methods!
 *
 *
 * 	We compute a bounding box for the mesh based on the first floating point
 * 	buffer passed as an attribute.  If this is not the right thing for you just
 * 	use \c compute_bounding_box_for_mesh or \c force_bounding_box_for_mesh
 * 	afterwards.
 *  \note When you pass 0 as vertex data to \c add_vertex_buffer_to_mesh the
 *  first non-zero float buffer is used for bounding box computation.
 */

/*! \file mesh.h
 * \ingroup meshes
 */


struct mesh {
	char *name;
	GLuint vao_id;
	GLuint *vertex_buffer_ids;
	GLuint index_buffer_id;
	unsigned int indices;
	unsigned int vertices;
	unsigned int vertex_buffers;
	char **vertex_buffer_names;
	int next_vbo;
	bool bound;
#if CGL_GL_VERSION == GLES2
	bool done;
	unsigned int *element_dim;
	GLenum *content_type;
#endif
	GLenum primitive_type;
	vec3f *bounding_box;
	bool keep_cpu_data;
	void **cpu_vertex_buffers;	//! we use void* here because there is actually no tellin...
	void *cpu_index_buffer;
};

// see http://gustedt.wordpress.com/2010/11/29/myth-and-reality-about-inline-in-c99/
extern inline bool equal_mesh_refs(mesh_ref a, mesh_ref b);

static struct mesh *meshes = 0;
static unsigned int meshes_allocated = 0, 
                    next_mesh_index = 0;

/*! \addtogroup meshes
 *  @{
 */

mesh_ref make_mesh(const char *name, unsigned int vertex_buffers) {
	// maintain mesh table
	if (next_mesh_index >= meshes_allocated) {
		struct mesh *old_array = meshes;
		unsigned int allocate = 1.5 * (meshes_allocated + 1);
		meshes = malloc(sizeof(struct mesh) * allocate);
		for (int i = 0; i < meshes_allocated; ++i)
			meshes[i] = old_array[i];
		meshes_allocated = allocate;
		free(old_array);
	}
	// set up new mesh
	mesh_ref ref;
	ref.id = next_mesh_index++;
	struct mesh *mesh = meshes+ref.id;
	mesh->next_vbo = 0;
	mesh->index_buffer_id = 0;
	mesh->indices = mesh->vertices = 0;
	mesh->bound = false;
	mesh->name = malloc(strlen(name)+1);
	strcpy(mesh->name, name);
	mesh->vertex_buffers = vertex_buffers;
	if (vertex_buffers) {
		mesh->vertex_buffer_ids = malloc(sizeof(GLuint) * vertex_buffers);
		mesh->vertex_buffer_names = malloc(sizeof(char*) * vertex_buffers);
		glGenBuffers(vertex_buffers, mesh->vertex_buffer_ids);
	}
#if CGL_GL == GLES
	else fprintf(stderr, "Warning: Meshes with external vertex buffers are not supported on gles, yet.\n");
#endif

#if CGL_GL == GL
	glGenVertexArrays(1, &mesh->vao_id);
#else
	mesh->done = false;
	mesh->element_dim = malloc(sizeof(unsigned int) * vertex_buffers);
	mesh->content_type = malloc(sizeof(GLenum) * vertex_buffers);
#endif
	mesh->primitive_type = GL_TRIANGLES;
	mesh->bounding_box = 0;
	mesh->keep_cpu_data = false;
	mesh->cpu_vertex_buffers = 0;
	mesh->cpu_index_buffer = 0;
	return ref;
}

void bind_mesh_to_gl(mesh_ref mr) {
#if CGL_GL == GL
	glBindVertexArray(meshes[mr.id].vao_id);
#else
	struct mesh *mesh = meshes+mr.id;
	if (mesh->done) {
		for (int i = 0; i < mesh->vertex_buffers; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_ids[i]);
			glVertexAttribPointer(i, mesh->element_dim[i], mesh->content_type[i], GL_FALSE, 0, 0);
			glEnableVertexAttribArray(i);
		}
		if (mesh->index_buffer_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer_id);
		}
	}
#endif
	meshes[mr.id].bound = true;
}

void unbind_mesh_from_gl(mesh_ref mr) {
#if CGL_GL == GL
	glBindVertexArray(0);
#else
	struct mesh *mesh = meshes+mr.id;
	if (mesh->done) {
		for (int i = 0; i < mesh->vertex_buffers; ++i)
			glDisableVertexAttribArray(i);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	if (!mesh->done)
		mesh->done = true;
#endif
	meshes[mr.id].bound = false;
}

int size_of_gl_type(GLenum content_type) {
	if (content_type == GL_FLOAT) 
		return sizeof(GLfloat);
	else if (content_type == GL_INT)
		return sizeof(int);
// 		return sizeof(GLint);
	else if (content_type == GL_UNSIGNED_INT)
		return sizeof(GLuint);
	else {
		fprintf(stderr, "unsupported enum for vertex buffer allocation/modification. suported are: GL_FLOAT, GL_INT, GL_UNSIGNED_INT.\n");
		exit(-1);
	}
}

void set_mesh_primitive_type(mesh_ref mr, GLenum type) {
	struct mesh *mesh = meshes+mr.id;
	mesh->primitive_type = type;
}

GLenum mesh_primitive_type(mesh_ref mr) {
	struct mesh *mesh = meshes+mr.id;
	return mesh->primitive_type;
}

GLuint mesh_vertex_buffer(mesh_ref mr, int id) {
	struct mesh *mesh = meshes+mr.id;
	return mesh->vertex_buffer_ids[id];
}

void compute_bounding_box_for_mesh(mesh_ref mr, unsigned int vertices, unsigned int element_dim, const float *data) {
	struct mesh *mesh = meshes+mr.id;
	mesh->bounding_box = malloc(sizeof(vec3f)*2);
	make_vec3f(mesh->bounding_box+0, FLT_MAX, FLT_MAX, FLT_MAX);
	make_vec3f(mesh->bounding_box+1, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < vertices; ++i) {
		int base = i * element_dim;
		if (data[base+0] < mesh->bounding_box[0].x)	mesh->bounding_box[0].x = data[base+0];
		if (data[base+0] > mesh->bounding_box[1].x)	mesh->bounding_box[1].x = data[base+0];
		if (element_dim > 1) {
			if (data[base+1] < mesh->bounding_box[0].y)	mesh->bounding_box[0].y = data[base+1];
			if (data[base+1] > mesh->bounding_box[1].y)	mesh->bounding_box[1].y = data[base+1];
			if (element_dim > 2) {
				if (data[base+2] < mesh->bounding_box[0].z)	mesh->bounding_box[0].z = data[base+2];
				if (data[base+2] > mesh->bounding_box[1].z)	mesh->bounding_box[1].z = data[base+2];
			}
		}
	}
}

void force_bounding_box_for_mesh(mesh_ref mr, const vec3f *min, const vec3f *max) {
	struct mesh *mesh = meshes+mr.id;
	if (mesh->bounding_box == 0)
		mesh->bounding_box = malloc(sizeof(vec3f)*2);
	copy_vec3f(mesh->bounding_box+0, min);
	copy_vec3f(mesh->bounding_box+1, max);
}

void bounding_box_of_mesh(mesh_ref mr, vec3f *min, vec3f *max) {
	struct mesh *mesh = meshes+mr.id;
	if (mesh->bounding_box == 0)
		min = max = 0;
	else {
		copy_vec3f(min, mesh->bounding_box+0);
		copy_vec3f(max, mesh->bounding_box+1);
	}
}

bool add_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, const void *data, GLenum usage_hint) {
	int unit_size = size_of_gl_type(content_type);
	struct mesh *mesh = meshes+mr.id;
	if (mesh->vertices) {
		if (mesh->vertices != vertices) {
			fprintf(stderr, "The mesh %s has a vbo of %d elements bound to it already, so you can't bind another one of size %d\n", mesh->name, mesh->vertices, vertices);
			exit(-1);
		}
	}
	else
		mesh->vertices = vertices;
	unsigned int size_in_bytes = unit_size * vertices * element_dim;
	int vbo_id = mesh->next_vbo++;
	if (vbo_id >= mesh->vertex_buffers) {
		fprintf(stderr, "Too many vbos bound to mesh %s.\n", mesh->name);
		return false;
	}
	mesh->vertex_buffer_names[vbo_id] = malloc(strlen(name)+1),
	strcpy(mesh->vertex_buffer_names[vbo_id], name);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_ids[vbo_id]);
	glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, usage_hint);
#if CGL_GL == GL
	glEnableVertexAttribArray(vbo_id);
	glVertexAttribPointer(vbo_id, element_dim, content_type, GL_FALSE, 0, 0);
#else
	mesh->element_dim[vbo_id] = element_dim;
	mesh->content_type[vbo_id] = content_type;
#endif
	if (mesh->keep_cpu_data) {
		mesh->cpu_vertex_buffers[vbo_id] = malloc(size_in_bytes);
		memcpy(mesh->cpu_vertex_buffers[vbo_id], data, size_in_bytes);
	}
	if (mesh->bounding_box == 0 && content_type == GL_FLOAT && data)
		compute_bounding_box_for_mesh(mr, vertices, element_dim, data);
	return true;
}

//! \attention this does not work with cpu side buffer copies!
bool add_existing_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, GLuint vboid) {
	struct mesh *mesh = meshes+mr.id;
	if (mesh->vertices) {
		if (mesh->vertices != vertices) {
			fprintf(stderr, "The mesh %s has a vbo of %d elements bound to it already, so you can't bind another one of size %d\n", mesh->name, mesh->vertices, vertices);
			exit(-1);
		}
	}
	else
		mesh->vertices = vertices;
	int vbo_id = mesh->next_vbo++;
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
#if CGL_GL == GL
	glEnableVertexAttribArray(vbo_id);
	glVertexAttribPointer(vbo_id, element_dim, content_type, GL_FALSE, 0, 0);
#else
	mesh->vertex_buffer_ids[vbo_id] = vboid;
	mesh->element_dim[vbo_id] = element_dim;
	mesh->content_type[vbo_id] = content_type;
#warning "adding existing vbos to meshes is experimental on gles, yet."
#endif
	return true;
}

bool change_vertex_buffer_data(mesh_ref mr, const char *name, GLenum content_type, unsigned int element_dim, const void *data, GLenum usage_hint) {
	struct mesh *mesh = meshes+mr.id;
	int vbo_id = -1;
	for (int i = 0; i < mesh->next_vbo; ++i)
		if (strcmp(name, mesh->vertex_buffer_names[i]) == 0) {
			vbo_id = i;
			break;
		}
	if (vbo_id == -1) {
		fprintf(stderr, "In change_vertex_buffer_data: Mesh does not have a vbo called %s.\n", name);
		exit(-1);
	}
	int unit_size = size_of_gl_type(content_type);
	unsigned int size_in_bytes = unit_size * mesh->vertices * element_dim;
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_ids[vbo_id]);
	glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, usage_hint);
#if CGL_GL == GL
	glEnableVertexAttribArray(vbo_id);
	glVertexAttribPointer(vbo_id, element_dim, content_type, GL_FALSE, 0, 0);
#endif
	return true;
}

void add_index_buffer_to_mesh(mesh_ref mr, unsigned int number_of_indices, const unsigned int *data, GLenum usage_hint) {
	struct mesh *mesh = meshes+mr.id;
	mesh->indices = number_of_indices;
	glGenBuffers(1, &mesh->index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * number_of_indices, data, usage_hint);
	if (mesh->keep_cpu_data) {
		mesh->cpu_index_buffer = malloc(sizeof(unsigned int) * number_of_indices);
		memcpy(mesh->cpu_index_buffer, data, sizeof(unsigned int) * number_of_indices);
	}
}

unsigned int index_buffer_length_of_mesh(mesh_ref mr) {
	return meshes[mr.id].indices;
}
unsigned int vertex_buffer_length_of_mesh(mesh_ref mr) {
	return meshes[mr.id].vertices;
}
char* mesh_name(mesh_ref mr) {
	return meshes[mr.id].name;
}

mesh_ref find_mesh(const char *name) {
	mesh_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_mesh_index; ++i)
		if (strcmp(meshes[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	return ref;
}

void draw_mesh(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	if (mesh->index_buffer_id)
		glDrawElements(mesh->primitive_type, mesh->indices, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(mesh->primitive_type, 0, mesh->vertices);
}

void draw_mesh_as(mesh_ref ref, GLenum primitive_type) {
	struct mesh *mesh = meshes+ref.id;
	if (mesh->index_buffer_id)
		glDrawElements(primitive_type, mesh->indices, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(primitive_type, 0, mesh->vertices);
}
	
/*! \brief setup mesh state to make a copy of data passed to add_*_buffer.
 *  \attention has to be setup before the buffers are added to the mes.
 */
void mesh_keep_cpu_data(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	mesh->keep_cpu_data = true;
	mesh->cpu_vertex_buffers = malloc(sizeof(float*) * mesh->vertex_buffers);
}

bool mesh_keeps_cpu_data(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	return mesh->keep_cpu_data;
}

void* cpu_index_buffer_of_mesh(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	return mesh->cpu_index_buffer;
}

void** cpu_vertex_buffers_of_mesh(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	return mesh->cpu_vertex_buffers;
}

unsigned int vertex_buffers_in_mesh(mesh_ref ref) {
	struct mesh *mesh = meshes+ref.id;
	return mesh->vertex_buffers;
}

//! @}

#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>
#include "scheme.h"

SCM_DEFINE(s_make_mesh, "make-mesh", 2, 0, 0, (SCM name, SCM vertex_buffers), "") {
	char *n = scm_to_locale_string(name);
	unsigned int b = scm_to_uint(vertex_buffers);
	mesh_ref ref = make_mesh(n, b);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_bind_mesh, "bind-mesh", 1, 0, 0, (SCM id), "") {
	mesh_ref ref = { scm_to_int(id) };
	bind_mesh_to_gl(ref);
	return id;
}

SCM_DEFINE(s_unbind_mesh, "unbind-mesh", 1, 0, 0, (SCM id), "") {
	mesh_ref ref = { scm_to_int(id) };
	unbind_mesh_from_gl(ref);
	return id;
}

SCM_DEFINE(s_find_mesh, "find-mesh", 1, 0, 0, (SCM name), "") {
	char *n = scm_to_locale_string(name);
	mesh_ref ref = find_mesh(n);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_mesh_name, "mesh-name", 1, 0, 0, (SCM id), "") {
	mesh_ref ref = { scm_to_int(id) };
	return scm_from_locale_string(mesh_name(ref));
}

SCM_DEFINE(s_add_vb_to_mesh, "add-vertex-buffer-to-mesh", 7, 0, 0, 
           (SCM meshid, SCM vb_name, SCM content_type, SCM vertices, SCM elem_dim, SCM data, SCM usage_hint), "") {
	// check the array first.
	if (!scm_is_array(data)) { 
		fprintf(stderr, "Data passed to add-vertex-buffer-to-mesh is not an array!\n"); 
		return SCM_BOOL_F; 
	}
	if (!scm_is_typed_array(data, scm_from_locale_symbol("f32"))) { 
		fprintf(stderr, "Data passed to add-vertex-buffer-to-mesh is not an array of type f32!\n"); 
		return SCM_BOOL_F; 
	}

	mesh_ref ref = { scm_to_int(meshid) };
	char *n = scm_to_locale_string(vb_name);
	GLenum content = scheme_symbol_to_gl_enum(&content_type);
	unsigned int verts = scm_to_uint(vertices);
	unsigned int elem_dimension = scm_to_uint(elem_dim);
	scm_t_array_handle handle;
	scm_array_get_handle(data, &handle);
	const float *float_data = scm_array_handle_f32_elements(&handle);
	GLenum usage = scheme_symbol_to_gl_enum(&usage_hint);

	add_vertex_buffer_to_mesh(ref, n, content, verts, elem_dimension, float_data, usage);
	
	scm_array_handle_release(&handle);
	free(n);
	return meshid;
}

SCM_DEFINE(s_draw_mesh, "draw-mesh", 1, 0, 0, (SCM id), "") {
	mesh_ref ref = { scm_to_int(id) };
	draw_mesh(ref);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_draw_mesh_as, "draw-mesh-as", 2, 0, 0, (SCM id, SCM prim_t), "") {
	mesh_ref ref = { scm_to_int(id) };
	GLenum prim = scheme_symbol_to_gl_enum(&prim_t);
	draw_mesh_as(ref, prim);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_set_mesh_prim_type, "set-mesh-primitive-type!", 2, 0, 0, (SCM id, SCM prim_t), "") {
	mesh_ref ref = { scm_to_int(id) };
	GLenum t = scm_to_int(prim_t);
	GLenum old = mesh_primitive_type(ref);
	set_mesh_primitive_type(ref, t);
	return scm_from_int(old);
}
	
void register_scheme_functions_for_meshes() {
#ifndef SCM_MAGIC_SNARFER
#include "mesh.x"
#endif
}

#endif

