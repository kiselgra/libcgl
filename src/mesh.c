#include "mesh.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
};

static struct mesh *meshes = 0;
static unsigned int meshes_allocated = 0, 
                    next_mesh_index = 0;

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
	mesh->vertex_buffer_ids = malloc(sizeof(GLuint) * vertex_buffers);
	mesh->vertex_buffers = vertex_buffers;
	mesh->vertex_buffer_names = malloc(sizeof(char*) * vertex_buffers);
	glGenVertexArrays(1, &mesh->vao_id);
	glGenBuffers(vertex_buffers, mesh->vertex_buffer_ids);
	return ref;
}

void bind_mesh_to_gl(mesh_ref mr) {
	glBindVertexArray(meshes[mr.id].vao_id);
	meshes[mr.id].bound = true;
}

void unbind_mesh_from_gl(mesh_ref mr) {
	glBindVertexArray(0);
	meshes[mr.id].bound = true;
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

bool add_vertex_buffer_to_mesh(mesh_ref mr, const char *name, GLenum content_type, unsigned int vertices, unsigned int element_dim, void *data, GLenum usage_hint) {
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
	if (vbo_id > mesh->vertex_buffers) {
		fprintf(stderr, "Too many vbos bound to mesh %s.\n", mesh->name);
		return false;
	}
	mesh->vertex_buffer_names[vbo_id] = malloc(strlen(name)+1),
	strcpy(mesh->vertex_buffer_names[vbo_id], name);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_ids[vbo_id]);
	glEnableVertexAttribArray(vbo_id);
	glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, usage_hint);
	glVertexAttribPointer(vbo_id, element_dim, content_type, GL_FALSE, 0, 0);
	return true;
}

bool change_vertex_buffer_data(mesh_ref mr, const char *name, GLenum content_type, unsigned int element_dim, void *data, GLenum usage_hint) {
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
	glEnableVertexAttribArray(vbo_id);
	glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, usage_hint);
	glVertexAttribPointer(vbo_id, element_dim, content_type, GL_FALSE, 0, 0);
	return true;
}

void add_index_buffer_to_mesh(mesh_ref mr, unsigned int number_of_indices, unsigned int *data, GLenum usage_hint) {
	struct mesh *mesh = meshes+mr.id;
	mesh->indices = number_of_indices;
	glGenBuffers(1, &mesh->index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * number_of_indices, data, usage_hint);
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
