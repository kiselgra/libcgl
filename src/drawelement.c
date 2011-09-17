#include "drawelement.h"

#include <stdlib.h>
#include <string.h>

struct drawelement {
	char *name;
	mesh_ref mesh;
	shader_ref shader;
	matrix4x4f local_transformation;
};

static struct drawelement *drawelements = 0;
static unsigned int drawelements_allocated = 0,
                    next_drawelement_index = 0;

drawelement_ref make_drawelement(const char *name, mesh_ref mesh, shader_ref shader, matrix4x4f *local_transformation) {
	if (next_drawelement_index >= drawelements_allocated) {
		struct drawelement *old_array = drawelements;
		unsigned int allocate = 1.5 * (drawelements_allocated + 1);
		drawelements = malloc(sizeof(struct drawelement) * allocate);
		for (int i = 0; i < drawelements_allocated; ++i)
			drawelements[i] = old_array[i];
		drawelements_allocated = allocate;
		free(old_array);
	}
	// actual setup
	drawelement_ref ref;
	ref.id = next_drawelement_index++;
	struct drawelement *drawelement = drawelements + ref.id;
	drawelement->name = malloc(strlen(name)+1);
	strcpy(drawelement->name, name);
	drawelement->mesh = mesh;
	drawelement->shader = shader;
	copy_matrix4x4f(&drawelement->local_transformation, local_transformation);
	return ref;
}



