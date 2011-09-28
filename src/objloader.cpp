#include "objloader.h"

#include "librc-test/vecs.h"

#include <libobjloader-0.0.1/objloader.h>
#include <libobjloader-0.0.1/default.h>

#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
using lib3dmath::mat4f;

extern "C" {
	void load_objfile(const char *name, obj_data *output) {
		obj_default::ObjFileLoader loader(name, "1 0 0 0  0 1 0 0  0 0 1 0  0 0 0 1");
		
		// copy raw vertex data
		output->vertices = loader.load_verts.size();
		output->vertex_data = (vec3f*)malloc(sizeof(vec3f) * output->vertices);
		for (int i = 0; i < output->vertices; ++i) {
			output->vertex_data[i].x = loader.load_verts[i].x;
			output->vertex_data[i].y = loader.load_verts[i].y;
			output->vertex_data[i].z = loader.load_verts[i].z;
			output->normal_data[i].x = loader.load_norms[i].x;
			output->normal_data[i].y = loader.load_norms[i].y;
			output->normal_data[i].z = loader.load_norms[i].z;
			output->texcoord_data[i].x = loader.load_texs[i].x;
			output->texcoord_data[i].y = loader.load_texs[i].y;
		}
		
		// copy vertex indices
		output->number_of_groups = loader.groups.size();
		output->groups = (group*)malloc(output->number_of_groups * sizeof(group));
		list<obj_default::ObjFileLoader::Group>::iterator it = loader.groups.begin();
		for (int i = 0; i < output->number_of_groups; ++i) {
			::group *group = &output->groups[i];
			group->indices = it->load_idxs_v.size();
			group->v_ids = (vec3i*)malloc(sizeof(vec3i) * group->indices);
			group->n_ids = (vec3i*)malloc(sizeof(vec3i) * group->indices);
			group->t_ids = (vec3i*)malloc(sizeof(vec3i) * group->indices);
			for (int j = 0; j < group->indices; ++j) {
				group->v_ids[j].x = it->load_idxs_v[j].x; group->v_ids[j].y = it->load_idxs_v[j].y; group->v_ids[j].z = it->load_idxs_v[j].z;
				group->n_ids[j].x = it->load_idxs_n[j].x; group->n_ids[j].y = it->load_idxs_n[j].y; group->n_ids[j].z = it->load_idxs_n[j].z;
				group->t_ids[j].x = it->load_idxs_t[j].x; group->t_ids[j].y = it->load_idxs_t[j].y; group->t_ids[j].z = it->load_idxs_t[j].z;
			}
			it++;
		}

		// missing: material stuff
	}
}

/* vim: set foldmethod=marker: */

