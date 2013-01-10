#ifndef __OBJLOADER_H__ 
#define __OBJLOADER_H__ 

#include "cgl-config.h"

#if LIBCGL_HAVE_LIBOBJLOADER == 1

#include <libmcm-0.0.1/vectors.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int x, y, z;
} vec3i;

typedef struct {
	char *name;
	vec4f col_amb, col_diff, col_spec;
	char *tex_a, *tex_d, *tex_s, *tex_bump;
} obj_mtl;

typedef struct {
	char *name;
	int triangles;
	vec3i *v_ids, *n_ids, *t_ids;
	obj_mtl *mtl;
} obj_group;

typedef struct {
	char *name;
	int vertices, normals, texcoords;
	vec3f *vertex_data;
	vec3f *normal_data;
	vec2f *texcoord_data;
	int number_of_groups;
	obj_group *groups;
	int number_of_materials;
	obj_mtl *materials;
} obj_data;


void load_objfile(const char *name, const char *filename, obj_data *output, bool inflate, bool collapse);


#ifdef __cplusplus
}
#endif

#endif

#endif

