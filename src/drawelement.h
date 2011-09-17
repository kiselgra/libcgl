#ifndef __DRAWELEMENT_H__ 
#define __DRAWELEMENT_H__ 

#include <libmcm-0.0.1/matrix.h>

#include "mesh.h"
#include "shader.h"

typedef struct {
	int id;
} drawelement_ref;

drawelement_ref make_drawelement(const char *name, mesh_ref mesh, shader_ref shader, matrix4x4f *local_transformation);

#endif

