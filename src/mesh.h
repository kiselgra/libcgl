#ifndef __MESH_H__ 
#define __MESH_H__ 

#include <stdbool.h>
#include <GL/glew.h>

typedef struct {
	int mesh_id;
} mesh_ref;

mesh_ref make_mesh(const char *name, unsigned int vertex_buffers);
void bind_mesh_to_gl(mesh_ref mr);
void unbind_mesh_from_gl(mesh_ref mr);
bool add_vertex_buffer_to_mesh(mesh_ref mr, char *name, GLenum content_type, unsigned int size_in_bytes, unsigned int element_dim, void *data, GLenum usage_hint);
void add_index_buffer_to_mesh(mesh_ref mr, unsigned int number_of_indices, unsigned int *data, GLenum usage_hint);

#endif

