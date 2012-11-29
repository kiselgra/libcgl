#ifndef __CAMERA_H__ 
#define __CAMERA_H__ 

#include <libmcm-0.0.1/camera-matrices.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int id;
} camera_ref;

camera_ref make_perspective_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far);
camera_ref make_orthographic_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, 
                                 float right, float left, float top, float bottom, float near, float far);
void delete_camera(camera_ref ref);

void change_projection_of_cam(camera_ref ref, float fovy, float aspect, float near, float far);
void change_lookat_of_cam(camera_ref ref, vec3f *pos, vec3f *dir, vec3f *up);
void recompute_gl_matrices_of_cam(camera_ref ref);

matrix4x4f* projection_matrix_of_cam(camera_ref ref);
matrix4x4f* lookat_matrix_of_cam(camera_ref ref);
matrix4x4f* gl_view_matrix_of_cam(camera_ref ref);
matrix4x4f* gl_normal_matrix_for_view_of(camera_ref ref);
float camera_near(camera_ref ref);
float camera_far(camera_ref ref);
float camera_fovy(camera_ref ref);
float camera_aspect(camera_ref ref);

void use_camera(camera_ref ref);
camera_ref current_camera(void);
const char* camera_name(camera_ref ref);
camera_ref find_camera(const char *name);
bool valid_camera_ref(camera_ref ref);
bool is_perspective_camera(camera_ref ref);

void camera_near_plane_size(camera_ref ref, vec2f *out);

#ifdef __cplusplus
}
#endif

#endif

