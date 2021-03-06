#include "objloader.h"

#if LIBCGL_HAVE_LIBOBJLOADER == 1
//#include "librc-test/vecs.h"

#include <libobjloader-0.0.1/objloader.h>
#include <libobjloader-0.0.1/default.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

using namespace std;

/*! \defgroup objloader Obj File Loader
 *
 *  This is just a converter from the representation of libobjloader to the structure \ref obj_data.
 *  Essentially bringing it down to the C level.
 */

/*! \file objloader.h
 *  \ingroup objloader
 *
 *  This is just a converter from the representation of libobjloader to the structure \ref obj_data.
 *  Essentially bringing it down to the C level.
 */

extern "C" {
	void load_objfile(const char *name, const char *filename, obj_data *output, bool inflate, bool collapse, float collapse_factor) {
		obj_default::ObjFileLoader loader(filename, "1 0 0 0  0 1 0 0  0 0 1 0  0 0 0 1");
		if (inflate) {
			loader.Inflate();
			if (collapse)
				loader.CollapseMaterials(collapse_factor);
		}
		output->name = (char*)malloc(strlen(name)+1);
		strcpy(output->name, name);
		
		// copy raw vertex data
		output->vertices = loader.load_verts.size();
		output->vertex_data = (vec3f*)malloc(sizeof(vec3f) * output->vertices);
		output->normals = loader.load_norms.size();
		output->normal_data = (vec3f*)malloc(sizeof(vec3f) * output->normals);
		bool with_tex = false;
		output->texcoords = 0;
		if (loader.load_texs.size() > 0)
			with_tex = true;
		if (with_tex) {
			output->texcoords = loader.load_texs.size();
			output->texcoord_data = (vec2f*)malloc(sizeof(vec2f) * output->texcoords);
		}
		else 
			output->texcoord_data = 0;
		
		for (int i = 0; i < output->vertices; ++i) {
			output->vertex_data[i].x = loader.load_verts[i].x;
			output->vertex_data[i].y = loader.load_verts[i].y;
			output->vertex_data[i].z = loader.load_verts[i].z;
		}
		for (int i = 0; i < output->normals; ++i) {
			output->normal_data[i].x = loader.load_norms[i].x;
			output->normal_data[i].y = loader.load_norms[i].y;
			output->normal_data[i].z = loader.load_norms[i].z;
		}
		for (int i = 0; i < output->texcoords; ++i) {
			output->texcoord_data[i].x = loader.load_texs[i].x;
			output->texcoord_data[i].y = loader.load_texs[i].y;
		}
		
		// copy material database
		output->materials = (obj_mtl*)malloc(sizeof(obj_mtl)*loader.materials.size());
		output->number_of_materials = loader.materials.size();
		int i = 0;
		for (std::map<std::string, ObjLoader::Mtl*>::iterator mit = loader.materials.begin(); mit != loader.materials.end(); ++mit) {
			std::string global_name = std::string(name) + "/" + mit->first;
			output->materials[i].name = (char*)malloc(global_name.length() + 1);
			strcpy(output->materials[i].name, global_name.c_str());
			make_vec4f(&output->materials[i].col_amb, mit->second->amb_r, mit->second->amb_g, mit->second->amb_b, mit->second->alpha);
			make_vec4f(&output->materials[i].col_diff, mit->second->dif_r, mit->second->dif_g, mit->second->dif_b, mit->second->alpha);
			make_vec4f(&output->materials[i].col_spec, mit->second->spe_r, mit->second->spe_g, mit->second->spe_b, mit->second->alpha);
			output->materials[i].spec_exp = mit->second->shininess;
			if (mit->second->tex_a != "") {
				output->materials[i].tex_a = (char*)malloc(mit->second->tex_a.length()+1);
				strcpy(output->materials[i].tex_a, mit->second->tex_a.c_str());
			}
			else output->materials[i].tex_a = 0;

			if (mit->second->tex_d != "") {
				output->materials[i].tex_d = (char*)malloc(mit->second->tex_d.length()+1);
				strcpy(output->materials[i].tex_d, mit->second->tex_d.c_str());
			}
			else output->materials[i].tex_d = 0;

			if (mit->second->tex_s != "") {
				output->materials[i].tex_s = (char*)malloc(mit->second->tex_s.length()+1);
				strcpy(output->materials[i].tex_s, mit->second->tex_s.c_str());
			}
			else output->materials[i].tex_s = 0;

			if (mit->second->tex_bump != "") {
				output->materials[i].tex_bump = (char*)malloc(mit->second->tex_bump.length()+1);
				strcpy(output->materials[i].tex_bump, mit->second->tex_bump.c_str());
			}
			else output->materials[i].tex_bump = 0;

			if (mit->second->tex_alpha != "") {
				output->materials[i].tex_alpha = (char*)malloc(mit->second->tex_alpha.length()+1);
				strcpy(output->materials[i].tex_alpha, mit->second->tex_alpha.c_str());
			}
			else output->materials[i].tex_alpha = 0;
			++i;
		}
		
		// copy vertex indices, link groups to materials
		output->number_of_groups = loader.groups.size();
		output->groups = (obj_group*)malloc(output->number_of_groups * sizeof(obj_group));
		list<obj_default::ObjFileLoader::Group>::iterator it = loader.groups.begin();
		for (int i = 0; i < output->number_of_groups; ++i) {
			::obj_group *group = &output->groups[i];
			bool group_with_tex = with_tex && it->load_idxs_t.size() && it->load_idxs_t[0].x != obj_default::ObjFileLoader::NOT_PRESENT;
			std::string gname = string(name) + "/" + it->name;
			group->name = (char*)malloc(gname.length()+1);
			strcpy(group->name, gname.c_str());
			group->triangles = it->load_idxs_v.size();
			group->v_ids = (vec3i*)malloc(sizeof(vec3i) * group->triangles);
			group->n_ids = (vec3i*)malloc(sizeof(vec3i) * group->triangles);
			group->t_ids = group_with_tex ? (vec3i*)malloc(sizeof(vec3i) * group->triangles) : 0;
			for (int j = 0; j < group->triangles; ++j) {
				group->v_ids[j].x = it->load_idxs_v[j].x;   group->v_ids[j].y = it->load_idxs_v[j].y;   group->v_ids[j].z = it->load_idxs_v[j].z;
				group->n_ids[j].x = it->load_idxs_n[j].x;   group->n_ids[j].y = it->load_idxs_n[j].y;   group->n_ids[j].z = it->load_idxs_n[j].z;
				if (group_with_tex) {
					group->t_ids[j].x = it->load_idxs_t[j].x;   group->t_ids[j].y = it->load_idxs_t[j].y;   group->t_ids[j].z = it->load_idxs_t[j].z;
				}
			}
			if (it->mat) {
				for (int i = 0; i < output->number_of_materials; ++i)
					if ((std::string(name) + "/" + it->mat->name) == std::string(output->materials[i].name)) {
						group->mtl = output->materials+i;
						break;
					}
			}
			else group->mtl = 0;
			it++;
		}
	}
}

#endif

/* vim: set foldmethod=marker: */

