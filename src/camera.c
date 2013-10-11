#include "camera.h"
#include "gl-version.h"

#include <stdlib.h>
#include <string.h>

/*! \defgroup cameras Cameras
 *
 * 	Mostly straight forward use of libmcm's camera matrices.
 *
 * 	The matrices are all stored separately, i.e. there is no modelview matrix.
 * 	You can, however, use one in the layer you build upon cgl -- of course ;)
 *
 * 	There is the concept of a `current camera' (see \ref current_camera).
 */

/*! \file camera.h
 *  \ingroup cameras
 */

struct camera {
	char *name;
	float near, far, aspect, fovy;
	matrix4x4f projection_matrix, 
			   lookat_matrix, 
			   gl_view_matrix,
			   gl_view_normal_matrix;
	vec3f internal_offset;
	float focus_plane, iod;
	void (*update_projection)(struct camera *);
};

#include "mm.h"
define_mm(camera, cameras, camera_ref);
#include "camera.xx"
	
void update_perspective_projection_of_cam(struct camera *camera);

/*! \addtogroup cameras
 *  @{
 */

camera_ref make_perspective_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far) {
	// maintainance
	camera_ref ref = allocate_camera_ref();
	// set up cam
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	make_vec3f(&camera->internal_offset, 0, 0, 0);
	strcpy(camera->name, name);
	camera->fovy = fovy;
	camera->aspect = aspect;
	camera->near = near;
	camera->far = far;
	camera->focus_plane = 0;
	camera->iod = 0;
	camera->update_projection = update_perspective_projection_of_cam;
	update_perspective_projection_of_cam(camera);
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

void make_frustum_matrixf(matrix4x4f *to, float t, float b, float l, float r, float near, float far) {
	#define row_col(r,c) col_major[(c)*4+(r)]
	to->row_col(0,0) = (2*near)/(r-l);
	to->row_col(1,0) = 0;
	to->row_col(2,0) = 0;
	to->row_col(3,0) = 0;

	to->row_col(0,1) = 0;
	to->row_col(1,1) = (2*near)/(t-b);
	to->row_col(2,1) = 0;
	to->row_col(3,1) = 0;

	to->row_col(0,2) = (r+l)/(r-l);
	to->row_col(1,2) = (t+b)/(t-b);
	to->row_col(2,2) = -(far+near)/(far-near);
	to->row_col(3,2) = -1;
	
	to->row_col(0,3) = 0;
	to->row_col(1,3) = 0;
	to->row_col(2,3) = -(2*far*near)/(far-near);
	to->row_col(3,3) = 0;
	#undef row_col
}

void update_perspective_stereo_projection_left(struct camera *camera) {
	float t = camera->near * tan((camera->fovy/2) * M_PI/180.0);
	float frustumshift = (camera->iod/2.0)*camera->near/camera->focus_plane;

	float b = -t;
	float r = camera->aspect * t;
	float l = -r + frustumshift;
	r += frustumshift;

	make_frustum_matrixf(&camera->projection_matrix, t, b, l, r, camera->near, camera->far);
	camera->projection_matrix.col_major[12] += camera->iod/2.0;
	make_vec3f(&camera->internal_offset, camera->iod/2.0, 0, 0);
// 	make_vec3f(&camera->internal_offset, 0, 0, 0);
}

void update_perspective_stereo_projection_right(struct camera *camera) {
	float t = camera->near * tan((camera->fovy/2) * M_PI/180.0);
	float frustumshift = (camera->iod/2.0)*camera->near/camera->focus_plane;

	float b = -t;
	float r = camera->aspect * t;
	float l = -r - frustumshift;
	r -= frustumshift;

	make_frustum_matrixf(&camera->projection_matrix, t, b, l, r, camera->near, camera->far);
	camera->projection_matrix.col_major[12] -= camera->iod/2.0;
	make_vec3f(&camera->internal_offset, -camera->iod/2.0, 0, 0);
// 	make_vec3f(&camera->internal_offset, 0, 0, 0);
}

camera_ref make_perspective_stereo_cam_left(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far, float iod, float focus) {
	camera_ref ref = allocate_camera_ref();
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	strcpy(camera->name, name);

	camera->fovy = fovy;
	camera->aspect = aspect;
	camera->near = near;
	camera->far = far;
	camera->iod = iod;
	camera->focus_plane = focus;
	camera->update_projection = update_perspective_stereo_projection_left;
	update_perspective_stereo_projection_left(camera);

	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

camera_ref make_perspective_stereo_cam_right(char *name, vec3f *pos, vec3f *dir, vec3f *up, float fovy, float aspect, float near, float far, float iod, float focus) {
	camera_ref ref = allocate_camera_ref();
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	strcpy(camera->name, name);

	camera->fovy = fovy;
	camera->aspect = aspect;
	camera->near = near;
	camera->far = far;
	camera->iod = iod;
	camera->focus_plane = focus;
	camera->update_projection = update_perspective_stereo_projection_right;
	update_perspective_stereo_projection_right(camera);

	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

camera_ref make_orthographic_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up, 
                                 float right, float left, float top, float bottom, float near, float far){
	// maintainance
	camera_ref ref = allocate_camera_ref();
	// set up cam
	struct camera *camera = cameras + ref.id;
	camera->name = malloc(strlen(name)+1);
	camera->near = near;
	camera->far = far;
	camera->aspect = (float)(top-bottom) / (float)(right-left);
	camera->fovy = 0;
	camera->update_projection = 0;
	make_vec3f(&camera->internal_offset, 0, 0, 0);
	strcpy(camera->name, name);
	make_orthographic_matrixf(&camera->projection_matrix, right, left, top, bottom, near, far);
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
	recompute_gl_matrices_of_cam(ref);
	return ref;
}

void delete_camera(camera_ref ref) {
	free_camera_ref(ref);
}

matrix4x4f* projection_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].projection_matrix; }
//! This is the lookat matrix as specified by pos/dir/up, not the GL view matrix.
matrix4x4f* lookat_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].lookat_matrix; }
//! This is the OpenGL view matrix for wc->ec.

matrix4x4f* gl_view_matrix_of_cam(camera_ref ref) { return &cameras[ref.id].gl_view_matrix; }	

matrix4x4f* gl_normal_matrix_for_view_of(camera_ref ref) { return &cameras[ref.id].gl_view_normal_matrix; }
float camera_near(camera_ref ref) { return cameras[ref.id].near; }
float camera_far(camera_ref ref)  { return cameras[ref.id].far; }

//! after changing any of the camera's parameters, you'll have to recompute the OpenGL matrices derived from them, to have effect on rendering.
void recompute_gl_matrices_of_cam(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
// 	matrix4x4f translate, res;
// 	make_unit_matrix4x4f(&translate);
// 	camera->lookat_matrix.col_major[12] = camera->internal_offset.x;
// 	camera->lookat_matrix.col_major[13] = camera->internal_offset.y;
// 	camera->lookat_matrix.col_major[14] = camera->internal_offset.z;
// 	multiply_matrices4x4f(&res, &translate, &camera->lookat_matrix);
// 	multiply_matrices4x4f(&res, &camera->lookat_matrix, &translate);

	make_gl_viewing_matrixf(&camera->gl_view_matrix, &camera->lookat_matrix);
// 	make_gl_viewing_matrixf(&camera->gl_view_matrix, &res);
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
	camera->fovy = fovy;
	camera->aspect = aspect;
	camera->near = near;
	camera->far = far;
	camera->update_projection(camera);
}

void change_near_far_of_cam(camera_ref ref, float near, float far) {
	struct camera *camera = cameras + ref.id;
	camera->near = near;
	camera->far = far;
	camera->update_projection(camera);
}

void change_stereo_camera_iod(camera_ref ref, float iod) {
	struct camera *camera = cameras + ref.id;
	camera->iod = iod;
	camera->update_projection(camera);
}


void update_perspective_projection_of_cam(struct camera *camera) {
	make_projection_matrixf(&camera->projection_matrix, camera->fovy, camera->aspect, camera->near, camera->far);
}

//! See \ref recompute_gl_matrices_of_cam.
void change_lookat_of_cam(camera_ref ref, vec3f *pos, vec3f *dir, vec3f *up) {
	struct camera *camera = cameras + ref.id;
	make_lookat_matrixf(&camera->lookat_matrix, pos, dir, up);
}

const char* camera_name(camera_ref ref) {
	struct camera *camera = cameras + ref.id;
    return camera->name;
}


/*! \brief When we have to refer to a camera, globally, we use the one which is considered current.
 * 	\note I think this is some kind of policy...
 */
static camera_ref current_cam_ref = { -1 };
//! Changes the `current camera'.
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

bool is_perspective_camera(camera_ref ref) {
	return camera_fovy(ref) != 0;
}

void camera_near_plane_size(camera_ref ref, vec2f *out) {
	float fovy_rad = camera_fovy(ref) * M_PI / 180.0f;
	float near_dist = camera_near(ref);
// 	out->y = 2*tanf(fovy_rad/2.0)*near_dist;	 // a=x/y
	out->y = tanf(fovy_rad)*near_dist;	 // a=x/y
	out->x = out->y * camera_aspect(ref);
}

void camera_far_plane_size(camera_ref ref, vec2f *out) {
	float fovy_rad = camera_fovy(ref) * M_PI / 180.0f;
	float far_dist = camera_far(ref);
// 	out->y = 2*tanf(fovy_rad/2.0)*far_dist;	 // a=x/y
	out->y = tanf(fovy_rad)*far_dist;	 // a=x/y
	out->x = out->y * camera_aspect(ref);
}

void add_normal_and_point_to_culling_data(int i, frustum_culling_t *data, vec3f *normal, vec3f *point) {
	data->normal[i] = *normal;
	normalize_vec3f(&data->normal[i]);
	data->dist[i] = -dot_vec3f(&data->normal[i], point);
}

void populate_frustum_culling_info(camera_ref ref, frustum_culling_t *data) {
	struct camera *camera = cameras + ref.id;
	float h_near = tan(camera->fovy * M_PI / 180.0f) * camera->near;
	float w_near = h_near * camera->aspect;
	vec3f tmp;
	vec3f cam_pos, cam_dir, cam_up;
	vec3f cam_right;
	extract_pos_vec3f_of_matrix(&cam_pos, &camera->lookat_matrix);
	extract_dir_vec3f_of_matrix(&cam_dir, &camera->lookat_matrix);
	extract_up_vec3f_of_matrix(&cam_up, &camera->lookat_matrix);
	cross_vec3f(&cam_right, &cam_dir, &cam_up);
	
	vec3f normal, point, in_plane, near_center, null = { 0,0,0 };
	
	// store cam
	data->cam = ref;

	// near
	normal = cam_dir;
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->near);
	add_components_vec3f(&point, &cam_pos, &tmp);
	near_center = point;
	add_normal_and_point_to_culling_data(0, data, &normal, &point);

	// far
	sub_components_vec3f(&normal, &null, &cam_dir);
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->far);
	add_components_vec3f(&point, &cam_pos, &tmp);
	add_normal_and_point_to_culling_data(1, data, &normal, &point);

	// right
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	add_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&normal, &cam_up, &in_plane);
	add_normal_and_point_to_culling_data(2, data, &normal, &cam_pos);

	// left
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	sub_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&normal, &in_plane, &cam_up);
	add_normal_and_point_to_culling_data(3, data, &normal, &cam_pos);

	// top
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	add_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&normal, &in_plane, &cam_right);
	add_normal_and_point_to_culling_data(4, data, &normal, &cam_pos);
		
	// bottom
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	sub_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&normal, &cam_right, &in_plane);
	add_normal_and_point_to_culling_data(5, data, &normal, &cam_pos);
}

typedef struct {
	camera_ref cam;
	vec3f point_on_plane; // camera eye, point on left,right,top,bottom
	vec3f near_center, far_center; // point on plane for near/far.
	vec3f normal_left, normal_right, normal_top, normal_bottom, normal_near, normal_far;
} frustum_culling_t_b;

void populate_frustum_culling_info_b(camera_ref ref, frustum_culling_t_b *data) {
	struct camera *camera = cameras + ref.id;
	float h_near = tan(camera->fovy * M_PI / 180.0f) * camera->near;
	float w_near = h_near * camera->aspect;
	vec3f tmp;
	vec3f cam_pos, cam_dir, cam_up;
	vec3f cam_right;
	extract_pos_vec3f_of_matrix(&cam_pos, &camera->lookat_matrix);
	extract_dir_vec3f_of_matrix(&cam_dir, &camera->lookat_matrix);
	extract_up_vec3f_of_matrix(&cam_up, &camera->lookat_matrix);
	cross_vec3f(&cam_right, &cam_dir, &cam_up);
	
	vec3f in_plane, null = { 0,0,0 };
	
	// store cam
	data->cam = ref;

	// point on plane for near and far.
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->near);
	add_components_vec3f(&data->near_center, &cam_pos, &tmp);
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->far);
	add_components_vec3f(&data->far_center, &cam_pos, &tmp);

	// far, near and common point for other planes
	sub_components_vec3f(&data->normal_near, &null, &cam_dir);
	data->normal_near = cam_dir;
	data->point_on_plane = cam_pos;

	// right
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	add_components_vec3f(&in_plane, &data->near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&data->normal_right, &cam_up, &in_plane);

	// left
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	sub_components_vec3f(&in_plane, &data->near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&data->normal_left, &in_plane, &cam_up);

	// top
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	add_components_vec3f(&in_plane, &data->near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&data->normal_top, &in_plane, &cam_right);
		
	// bottom
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	sub_components_vec3f(&in_plane, &data->near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&data->normal_bottom, &cam_right, &in_plane);
}

bool aabb_in_frustum(frustum_culling_t *culling_data, vec3f *bb_min, vec3f *bb_max) {
	
	for (int i = 0; i < 6; ++i) {
		vec3f p, n;
		vec3f *normal = &culling_data->normal[i];

		if (normal->x >= 0) p.x = bb_max->x; else p.x = bb_min->x;
		if (normal->y >= 0) p.y = bb_max->y; else p.y = bb_min->y;
		if (normal->z >= 0) p.z = bb_max->z; else p.z = bb_min->z;

		if (normal->x >= 0) n.x = bb_min->x; else n.x = bb_max->x;
		if (normal->y >= 0) n.y = bb_min->y; else n.y = bb_max->y;
		if (normal->z >= 0) n.z = bb_min->z; else n.z = bb_max->z;

		float distance = culling_data->dist[i] + dot_vec3f(&culling_data->normal[i], &p);
		if (distance < 0)
			return false;
	}
	return true;
}

bool point_in_frustum(camera_ref ref, vec3f *point) {
	struct camera *camera = cameras + ref.id;
	float h_near = tan(camera->fovy * M_PI / 180.0f) * camera->near;
	float w_near = h_near * camera->aspect;
	vec3f tmp;
	vec3f far_center, near_center;
	vec3f cam_pos, cam_dir, cam_up;
	vec3f cam_right;
	extract_pos_vec3f_of_matrix(&cam_pos, &camera->lookat_matrix);
	extract_dir_vec3f_of_matrix(&cam_dir, &camera->lookat_matrix);
	extract_up_vec3f_of_matrix(&cam_up, &camera->lookat_matrix);
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->near);
	add_components_vec3f(&near_center, &cam_pos, &tmp);
	mul_vec3f_by_scalar(&tmp, &cam_dir, camera->far);
	add_components_vec3f(&far_center, &cam_pos, &tmp);
	cross_vec3f(&cam_right, &cam_dir, &cam_up);

	vec3f plane_normal, to_point, in_plane, null = { 0,0,0 };
	
	// check far
	sub_components_vec3f(&plane_normal, &null, &cam_dir);
	sub_components_vec3f(&to_point, point, &far_center);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;

	// check near
	plane_normal = cam_dir;
	sub_components_vec3f(&to_point, point, &near_center);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;


	// right
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	add_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&plane_normal, &cam_up, &in_plane);
	sub_components_vec3f(&to_point, point, &cam_pos);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;

	// left
	mul_vec3f_by_scalar(&tmp, &cam_right, w_near);
	sub_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&plane_normal, &in_plane, &cam_up);
	sub_components_vec3f(&to_point, point, &cam_pos);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;

	// top
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	add_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&plane_normal, &in_plane, &cam_right);
	sub_components_vec3f(&to_point, point, &cam_pos);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;
		
	// bottom
	mul_vec3f_by_scalar(&tmp, &cam_up, h_near);
	sub_components_vec3f(&in_plane, &near_center, &tmp);
	sub_components_vec3f(&in_plane, &in_plane, &cam_pos);
	normalize_vec3f(&in_plane);
	cross_vec3f(&plane_normal, &cam_right, &in_plane);
	sub_components_vec3f(&to_point, point, &cam_pos);
	if (dot_vec3f(&plane_normal, &to_point) < 0)
		return false;
	
	return true;
}

bool aabb_in_camera_frustum(camera_ref ref, vec3f *bb_min, vec3f *bb_max) {
	
	vec3f points[] = { { bb_min->x, bb_min->y, bb_min->z },
	                   { bb_min->x, bb_min->y, bb_max->z },
	                   { bb_min->x, bb_max->y, bb_min->z },
	                   { bb_min->x, bb_max->y, bb_max->z },
	                   { bb_max->x, bb_min->y, bb_min->z },
	                   { bb_max->x, bb_min->y, bb_max->z },
	                   { bb_max->x, bb_max->y, bb_min->z },
	                   { bb_max->x, bb_max->y, bb_max->z } };

	for (int i = 0; i < 8; ++i) {
		if (point_in_frustum(ref, points+i))
			return true;
	}
	return false;


	/*
	#define row_col(r,c) col_major[(c)*4+(r)]
	#define A(r,c) clipspace.row_col(r-1,c-1)
	// #define A(i,j) clipspace.row_col(i-1,j-1)

	matrix4x4f clipspace;
	multiply_matrices4x4f(&clipspace, &camera->projection_matrix, &camera->gl_view_matrix);

	vec4f left_plane = { A(4,1) + A(1,1),
	                     A(4,2) + A(1,2),
	                     A(4,3) + A(1,3),
	                     A(4,4) + A(1,4) };

	vec4f right_plane = { A(4,1) - A(1,1),
	                      A(4,2) - A(1,2),
	                      A(4,3) - A(1,3),
	                      A(4,4) - A(1,4) };

	vec4f bottom_plane = { A(4,1) + A(2,1),
	                       A(4,2) + A(2,2),
	                       A(4,3) + A(2,3),
	                       A(4,4) + A(2,4) };


	vec4f top_plane = { A(4,1) - A(2,1),
	                    A(4,2) - A(2,2),
	                    A(4,3) - A(2,3),
	                    A(4,4) - A(2,4) };

	vec4f near_plane = { A(4,1) + A(3,1),
	                     A(4,2) + A(3,2),
	                     A(4,3) + A(3,3),
	                     A(4,4) + A(3,4) };

	vec4f far_plane = { A(4,1) - A(3,1),
	                    A(4,2) - A(3,2),
	                    A(4,3) - A(3,3),
	                    A(4,4) - A(3,4) };
	
	vec4f points[] = { { bb_min->x, bb_min->y, bb_min->z, 1 },
	                   { bb_min->x, bb_min->y, bb_max->z, 1 },
	                   { bb_min->x, bb_max->y, bb_min->z, 1 },
	                   { bb_min->x, bb_max->y, bb_max->z, 1 },
	                   { bb_max->x, bb_min->y, bb_min->z, 1 },
	                   { bb_max->x, bb_min->y, bb_max->z, 1 },
	                   { bb_max->x, bb_max->y, bb_min->z, 1 },
	                   { bb_max->x, bb_max->y, bb_max->z, 1 } };

	for (int i = 0; i < 8; ++i) {
		float d_left, d_right, d_top, d_bottom, d_near, d_far;

		d_left   = dot_vec4f(&left_plane,   &points[i]);
		d_right  = dot_vec4f(&right_plane,  &points[i]);
		d_top    = dot_vec4f(&top_plane,    &points[i]);
		d_bottom = dot_vec4f(&bottom_plane, &points[i]);
		d_near   = dot_vec4f(&near_plane,   &points[i]);
		d_far    = dot_vec4f(&far_plane,    &points[i]);
		if (d_left < 0 || d_right < 0 || d_top < 0 || d_bottom < 0 || d_near < 0 || d_far < 0)
			return false;
	}
	*/
	
	return true;

// 	float distanceLeft = dot_vec4f(&leftPlane, &chunkCenter);
// 	float distanceRight = dot_vec4f(&rightPlane, &chunkCenter);
// 	float distanceBottom = dot_vec4f(&bottomPlane, &chunkCenter);
// 	float distanceTop = dot_vec4f(&topPlane, &chunkCenter);
// 	float distanceNear = dot_vec4f(&nearPlane, &chunkCenter);
// 	float distanceFar = dot_vec4f(&farPlane, &chunkCenter);

	
}

//! @}

#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>
#include "scheme.h"
	
static void load_vec3f(SCM list, vec3f *v, const char *info) {
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

SCM_DEFINE(s_make_ortho_cam, "make-orthographic-camera", 10, 0, 0, (SCM name, SCM pos, SCM dir, SCM up, SCM right, SCM left, SCM top, SCM bottom,  SCM near, SCM far), "") {
	char *n = scm_to_locale_string(name);
	vec3f p, d, u;
	make_vec3f(&p, 0, 0, 0);
	make_vec3f(&d, 0, 0, 0);
	make_vec3f(&u, 0, 0, 0);
	load_vec3f(pos, &p, "pos");
	load_vec3f(dir, &d, "dir");
	load_vec3f(up, &u, "up");
	float r = (float)scm_to_double(right);
	float l = (float)scm_to_double(left);
	float t = (float)scm_to_double(top);
	float b = (float)scm_to_double(bottom);
	float ne = (float)scm_to_double(near);
	float fa = (float)scm_to_double(far);
	camera_ref ref = make_orthographic_cam(n, &p, &d, &u, r, l, t, b, ne, fa);
	free(n);
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_make_perspective_cam, "make-perspective-camera", 8, 0, 0, (SCM name, SCM pos, SCM dir, SCM up, SCM fovy, SCM aspect, SCM near, SCM far), "") {
	char *n = scm_to_locale_string(name);
	vec3f p, d, u;
	make_vec3f(&p, 0, 0, 0);
	make_vec3f(&d, 0, 0, 0);
	make_vec3f(&u, 0, 0, 0);
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

SCM_DEFINE(s_del_cam, "delete-camera", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	delete_camera(ref);
	return SCM_BOOL_T;
}

SCM_DEFINE(s_pos_of_cam, "cam-pos", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	matrix4x4f *lookat = lookat_matrix_of_cam(ref);
	vec3f v;
	extract_pos_vec3f_of_matrix(&v, lookat);
	return vec3f_to_list(&v);
}

SCM_DEFINE(s_dir_of_cam, "cam-dir", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	matrix4x4f *lookat = lookat_matrix_of_cam(ref);
	vec3f v;
	extract_dir_vec3f_of_matrix(&v, lookat);
	return vec3f_to_list(&v);
}

SCM_DEFINE(s_up_of_cam, "cam-up", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	matrix4x4f *lookat = lookat_matrix_of_cam(ref);
	vec3f v;
	extract_up_vec3f_of_matrix(&v, lookat);
	return vec3f_to_list(&v);
}

SCM_DEFINE(s_curr_cam, "current-camera", 0, 0, 0, (), "") {
	camera_ref ref = current_camera();
	return scm_from_int(ref.id);
}

SCM_DEFINE(s_cam_near, "cam-near", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	return scm_from_double(camera_near(ref));
}

SCM_DEFINE(s_cam_far, "cam-far", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	return scm_from_double(camera_far(ref));
}

SCM_DEFINE(s_find_cam, "find-camera", 1, 0, 0, (SCM name), "") {
	char *n = scm_to_locale_string(name);
	camera_ref ref = find_camera(n);
	free(n);
	if (valid_camera_ref(ref))
		return scm_from_int(ref.id);
	return SCM_BOOL_F;
}

static SCM get_cam_matrix(SCM id, matrix4x4f*(*func)(camera_ref ref)) {
	camera_ref ref = { scm_to_int(id) };
	return matrix4x4f_to_scm(func(ref));
}

SCM_DEFINE(s_proj_matrix_of_cam, "projection-matrix-of-cam", 1, 0, 0, (SCM id), "") { return get_cam_matrix(id, projection_matrix_of_cam); }
SCM_DEFINE(s_lookat_matrix_of_cam, "lookat-matrix-of-cam", 1, 0, 0, (SCM id), "") { return get_cam_matrix(id, lookat_matrix_of_cam); }
SCM_DEFINE(s_gl_view_matrix_of_cam, "gl-view-matrix-of-cam", 1, 0, 0, (SCM id), "") { return get_cam_matrix(id, gl_view_matrix_of_cam); }
SCM_DEFINE(s_gl_normal_matrix_of_cam, "gl-normal-matrix-of-cam", 1, 0, 0, (SCM id), "") { return get_cam_matrix(id, gl_normal_matrix_for_view_of); }

static void set_cam_matrix(SCM id, matrix4x4f*(*func)(camera_ref ref), SCM mat) {
	camera_ref ref = { scm_to_int(id) };
	scm_to_matrix4x4f(func(ref), mat);
}

SCM_DEFINE(s_proj_matrix_of_cam_x, "set-projection-matrix-of-cam!", 2, 0, 0, (SCM id, SCM bv), "") { set_cam_matrix(id, projection_matrix_of_cam, bv); return SCM_BOOL_T; }
SCM_DEFINE(s_lookat_matrix_of_cam_x, "set-lookat-matrix-of-cam!", 2, 0, 0, (SCM id, SCM bv), "") { set_cam_matrix(id, lookat_matrix_of_cam, bv); return SCM_BOOL_T; }
SCM_DEFINE(s_gl_view_matrix_of_cam_x, "set-gl-view-matrix-of-cam!", 2, 0, 0, (SCM id, SCM bv), "") { set_cam_matrix(id, gl_view_matrix_of_cam, bv); return SCM_BOOL_T; }
SCM_DEFINE(s_gl_normal_matrix_of_cam_x, "set-gl-normal-matrix-of-cam!", 2, 0, 0, (SCM id, SCM bv), "") { set_cam_matrix(id, gl_normal_matrix_for_view_of, bv); return SCM_BOOL_T; }

SCM_DEFINE(s_recompute_gl_matrices_of_cam, "recompute-gl-matrices-of-cam", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	recompute_gl_matrices_of_cam(ref);
	return SCM_BOOL_T;
}


void s_uniform_cam_any_matrix(SCM loc, matrix4x4f *m) {
	int location = scm_to_int(loc);
	glUniformMatrix4fv(location, 1, GL_FALSE, m->col_major);
}

SCM_DEFINE(s_uniform_cam_matrix_p, "uniform-camera-proj-matrix", 2, 0, 0, (SCM loc, SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	s_uniform_cam_any_matrix(loc, projection_matrix_of_cam(ref));
	return SCM_BOOL_T;
}
SCM_DEFINE(s_uniform_cam_matrix_v, "uniform-camera-view-matrix", 2, 0, 0, (SCM loc, SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	s_uniform_cam_any_matrix(loc, gl_view_matrix_of_cam(ref));
	return SCM_BOOL_T;
}
SCM_DEFINE(s_uniform_cam_matrix_n, "uniform-camera-normal-matrix", 2, 0, 0, (SCM loc, SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	s_uniform_cam_any_matrix(loc, gl_normal_matrix_for_view_of(ref));
	return SCM_BOOL_T;
}
SCM_DEFINE(s_cam_name, "camera-name", 1, 0, 0, (SCM id), "") {
	camera_ref ref = { scm_to_int(id) };
	return scm_from_locale_string(camera_name(ref));
}


void register_scheme_functions_for_cameras() {
#ifndef SCM_MAGIC_SNARFER
#include "camera.x"
#endif
}
#endif
