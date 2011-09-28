#ifndef __OBJLOADER_H__ 
#define __OBJLOADER_H__ 

#include <libmcm-0.0.1/vectors.h>

typedef struct {
	int x, y, z;
} vec3i;

typedef struct {
	int indices;
	vec3i *v_ids, *n_ids, *t_ids;
} group;

typedef struct {
	int vertices;
	vec3f *vertex_data;
	vec3f *normal_data;
	vec2f *texcoord_data;
	int number_of_groups;
	group *groups;
} obj_data;


#ifdef __cplusplus
extern "C" {
#endif

	void load_objfile(const char *name, obj_data *output);

#ifdef __cplusplus
}
#endif

#endif

