#ifndef __MESH_H__ 
#define __MESH_H__ 

#include <stdbool.h>
#include "cgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libmcm/vectors.h>

typedef struct {
	int id;
} mesh_ref;

inline bool equal_mesh_refs(mesh_ref a, mesh_ref b) {
	return a.id == b.id;
}

mesh_ref make_mesh(const char *name, unsigned int vertex_buffers);
void bind_mesh_to_gl(mesh_ref mr);
void unbind_mesh_from_gl(mesh_ref mr);
bool add_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, const void *data, GLenum usage_hint);
bool add_existing_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, GLuint vboid);
bool change_vertex_buffer_data(mesh_ref mr, const char *name, GLenum content_type, unsigned int element_dim, const void *data, GLenum usage_hint);
void add_index_buffer_to_mesh(mesh_ref mr, unsigned int number_of_indices, const unsigned int *data, GLenum usage_hint);
unsigned int index_buffer_length_of_mesh(mesh_ref mr);
unsigned int vertex_buffer_length_of_mesh(mesh_ref mr);
char* mesh_name(mesh_ref mr);
mesh_ref find_mesh(const char *name);
void draw_mesh(mesh_ref ref);//!< attention: very general, your own draw call might be faster.
void draw_mesh_as(mesh_ref mr, GLenum primitive_type);//!< ditto.
GLenum mesh_primitive_type(mesh_ref mr);
void set_mesh_primitive_type(mesh_ref mr, GLenum type);
GLuint mesh_vertex_buffer(mesh_ref mr, int id);
void bounding_box_of_mesh(mesh_ref mr, vec3f *min, vec3f *max);
void force_bounding_box_for_mesh(mesh_ref mr, const vec3f *min, const vec3f *max);
void compute_bounding_box_for_mesh(mesh_ref mr, unsigned int vertices, unsigned int element_dim, const float *data);
void mesh_keep_cpu_data(mesh_ref ref);
bool mesh_keeps_cpu_data(mesh_ref ref);
unsigned int vertex_buffers_in_mesh(mesh_ref ref);
void** cpu_vertex_buffers_of_mesh(mesh_ref ref);
void* cpu_index_buffer_of_mesh(mesh_ref ref);
const char* mesh_vertex_buffer_name(mesh_ref ref, int n);
GLenum mesh_vertex_buffer_content_type(mesh_ref ref, int n);
int mesh_vertex_buffer_element_dim(mesh_ref ref, int n);

bool valid_mesh_ref(mesh_ref ref);

define_array(mesh);

#ifdef __cplusplus
}
#endif

#endif

