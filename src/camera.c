#include "camera.h"

#include <stdlib.h>
#include <string.h>

struct camera {
	char *name;
	matrix4x4f projection_matrix, 
			   lookat_matrix, 
			   gl_view_matrix;
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
	make_projection_matrixf(&camera->projection_matrix, 45, 1, 0.01, 10000);
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
	strcpy(camera->name, name);
	make_orthographic_matrixf(&camera->projection_matrix, right, left, top, bottom, near, far);
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

matrix4x4f* projection_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].projection_matrix; }
matrix4x4f* lookat_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].lookat_matrix; }
matrix4x4f* gl_view_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].gl_view_matrix; }

void recompute_gl_matrices_of_cam(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
	make_gl_viewing_matrixf(&camera->gl_view_matrix, &camera->lookat_matrix);
}

static camera_ref current_cam_ref = { -1 };
void use_camera(camera_ref ref) {
	current_cam_ref = ref;
}
camera_ref current_camera() {
	return current_cam_ref;
}
