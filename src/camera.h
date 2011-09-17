#ifndef __CAMERA_H__ 
#define __CAMERA_H__ 

#include <libmcm-0.0.1/camera-matrices.h>

typedef struct {
	int id;
} camera_ref;

camera_ref make_perspective_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far);

matrix4x4f* projection_matrix_of_cam(camera_ref ref);
matrix4x4f* lookat_matrix_of_cam(camera_ref ref);
matrix4x4f* gl_view_matrix_of_cam(camera_ref ref);

void recompute_gl_matrices_of_cam(camera_ref ref);

void use_camera(camera_ref ref);
camera_ref current_camera();

#endif

