#include "camera.h"

#include <stdlib.h>
#include <string.h>

struct camera {
	char *name;
	float near, far, aspect, fovy;
	matrix4x4f projection_matrix, 
			   lookat_matrix, 
			   gl_view_matrix,
			   gl_view_normal_matrix;
};

static struct camera *cameras = 0;
static unsigned int cameras_allocated = 0,
					next_camera_index = 0;

static void allocate_cam() {
	if (next_camera_index >= cameras_allocated) {
		struct camera *old_array = cameras;
		unsigned int allocate = 1.5 * (cameras_allocated + 1);
		cameras = malloc(sizeof(struct camera) * allocate);
		for (int i = 0; i < cameras_allocated; ++i)
			cameras[i] = old_array[i];
		cameras_allocated = allocate;
		free(old_array);
	}
}

camera_ref make_perspective_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far) {
	// maintainance
	allocate_cam();
	// set up cam
	camera_ref ref;
	ref.id = next_camera_index++;
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	strcpy(camera->name, name);
	change_projection_of_cam(ref, fovy, aspect, near, far);
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

camera_ref make_orthographic_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, 
                                 float right, float left, float top, float bottom, float near, float far){
	// maintainance
	allocate_cam();
	// set up cam
	camera_ref ref;
	ref.id = next_camera_index++;
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	camera->near = near;
	camera->far = far;
	camera->aspect = (float)(top-bottom) / (float)(right-left);
	camera->fovy = 0;
	strcpy(camera->name, name);
	make_orthographic_matrixf(&camera->projection_matrix, right, left, top, bottom, near, far);
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

matrix4x4f* projection_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].projection_matrix; }
matrix4x4f* lookat_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].lookat_matrix; }
matrix4x4f* gl_view_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].gl_view_matrix; }
matrix4x4f* gl_normal_matrix_for_view_of(camera_ref ref) { return &cameras[ref.id].gl_view_normal_matrix; }
float camera_near(camera_ref ref) { return cameras[ref.id].near; }
float camera_far(camera_ref ref)  { return cameras[ref.id].far; }

void recompute_gl_matrices_of_cam(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
	make_gl_viewing_matrixf(&camera->gl_view_matrix, &camera->lookat_matrix);
	matrix4x4f tmp;
	invert_matrix4x4f(&tmp, &camera->gl_view_matrix);
	transpose_matrix4x4f(&camera->gl_view_normal_matrix, &tmp);
}

float camera_fovy(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
	return camera->fovy;
}

float camera_aspect(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
	return camera->aspect;
}

void change_projection_of_cam(camera_ref ref, float fovy, float aspect, float near, float far) {
	struct camera *camera = cameras + ref.id;
	camera->near = near;
	camera->far = far;
	camera->aspect = aspect;
	camera->fovy = fovy;
	make_projection_matrixf(&camera->projection_matrix, fovy, aspect, near, far);
}

void change_lookat_of_cam(camera_ref ref, vec3f *pos, vec3f *dir, vec3f *up) {
	struct camera *camera = cameras + ref.id;
	make_lookat_matrixf(&cameras->lookat_matrix, pos, dir, up);
}



static camera_ref current_cam_ref = { -1 };
void use_camera(camera_ref ref) {
	current_cam_ref = ref;
}
camera_ref current_camera() {
	return current_cam_ref;
}

camera_ref find_camera(const char *name) {
	camera_ref ref = { -1 };
	if (strlen(name) == 0) return ref;
	for (int i = 0; i < next_camera_index; ++i)
		if (strcmp(cameras[i].name, name) == 0) {
			ref.id = i;
			return ref;
		}
	return ref;
}

bool valid_camera_ref(camera_ref ref) {
	return ref.id >= 0;
}

bool is_perspective_camera(camera_ref ref) {
	return camera_fovy(ref) != 0;
}


#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>

SCM_DEFINE(s_make_perspective_cam, "make-perspective-camera", 8, 0, 0, (SCM name, SCM pos, SCM dir, SCM up, SCM fovy, SCM aspect, SCM near, SCM far), "") {
	char *n = scm_to_locale_string(name);
	vec3f p, d, u;
	make_vec3f(&p, 0, 0, 0);
	make_vec3f(&d, 0, 0, 0);
	make_vec3f(&u, 0, 0, 0);
	void load_vec3f(SCM list, vec3f *v, const char *info) {
		if (!scm_list_p(list)) {
			fprintf(stderr, "%s is not a list.\n", info);
			return;
		}
		if (scm_to_int(scm_length(list)) != 3) {
			fprintf(stderr, "%s must be a list of length 3 (a vec3).\n", info);
			return;
		}
		v->x = (float)scm_to_double(scm_list_ref(list, scm_from_int(0)));
		v->y = (float)scm_to_double(scm_list_ref(list, scm_from_int(1)));
		v->z = (float)scm_to_double(scm_list_ref(list, scm_from_int(2)));
	}
	load_vec3f(pos, &p, "pos");
	load_vec3f(dir, &d, "dir");
	load_vec3f(up, &u, "up");
	float fo = (float)scm_to_double(fovy);
	float as = (float)scm_to_double(aspect);
	float ne = (float)scm_to_double(near);
	float fa = (float)scm_to_double(far);
	camera_ref ref = make_perspective_cam(n, &p, &d, &u, fo, as, ne, fa);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_use_cam, "use-camera", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	use_camera(ref);
	return SCM_BOOL_T;
}

void register_scheme_functions_for_cameras() {
#ifndef SCM_MAGIC_SNARFER
#include "camera.x"
#endif
}
#endif
