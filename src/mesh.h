#ifndef __MESH_H__ 
#define __MESH_H__ 

#include <stdbool.h>
#include "cgl.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int id;
} mesh_ref;

mesh_ref make_mesh(const char *name, unsigned int vertex_buffers);
void bind_mesh_to_gl(mesh_ref mr);
void unbind_mesh_from_gl(mesh_ref mr);
bool add_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, void *data, GLenum usage_hint);
bool change_vertex_buffer_data(mesh_ref mr, const char *name, GLenum content_type, unsigned int element_dim, void *data, GLenum usage_hint);
void add_index_buffer_to_mesh(mesh_ref mr, unsigned int number_of_indices, unsigned int *data, GLenum usage_hint);
unsigned int index_buffer_length_of_mesh(mesh_ref mr);
unsigned int vertex_buffer_length_of_mesh(mesh_ref mr);
char* mesh_name(mesh_ref mr);


#ifdef __cplusplus
}
#endif

#endif

