#include "glut.h"
#include "camera.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>

#include <GL/freeglut.h>
#include <stdio.h>

void startup_glut(const char *title, int argc, char **argv, int gl_maj, int gl_min, int res_x, int res_y)
{
	glutInit(&argc, argv);
	glutInitContextVersion (gl_maj, gl_min);
	glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (res_x, res_y); 
	glutInitWindowPosition (100, 100);
	if (title) glutCreateWindow(title);
	else       glutCreateWindow("a gl window");
	
	glViewport(0,0,res_x,res_y);

	glutMouseFunc(standard_mouse_func);
	glutMotionFunc(standard_mouse_motion);
	// glutReshapeFunc(reshape);
	// glutReshapeFunc(reshape);
	glutKeyboardFunc (standard_keyboard);
}

void register_display_function(     void (*fn)())                        { glutDisplayFunc(fn); }
void register_idle_function(        void (*fn)())                        { glutIdleFunc(fn); }
void register_keyboard_function(    void (*fn)(unsigned char, int, int)) { glutKeyboardFunc(fn); }
void register_mouse_motion_function(void (*fn)(int, int))                { glutMotionFunc(fn); }
void register_mouse_function(       void (*fn)(int, int, int, int))      { glutMouseFunc(fn); }

void standard_keyboard(unsigned char key, int x, int y)
{
	vec3f tmp;
	vec3f cam_right, cam_pos, cam_dir, cam_up;
	matrix4x4f *lookat_matrix = lookat_matrix_of_cam(current_camera());
	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix);
	extract_dir_vec3f_of_matrix(&cam_dir, lookat_matrix);
	extract_up_vec3f_of_matrix(&cam_up, lookat_matrix);
	extract_right_vec3f_of_matrix(&cam_right, lookat_matrix);
	float move_factor = 0.1;
	switch (key) {
		case 27:
			exit(0);
		case 's':
			copy_vec3f(&tmp, &cam_dir);
			mul_vec3f_by_scalar(&tmp, &tmp, -move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'w':
			copy_vec3f(&tmp, &cam_dir);
			mul_vec3f_by_scalar(&tmp, &tmp, move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'a':
			copy_vec3f(&tmp, &cam_right);
			mul_vec3f_by_scalar(&tmp, &tmp, -move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'd':
			copy_vec3f(&tmp, &cam_right);
			mul_vec3f_by_scalar(&tmp, &tmp, move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'f':
			copy_vec3f(&tmp, &cam_up);
			mul_vec3f_by_scalar(&tmp, &tmp, -move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'r':
			copy_vec3f(&tmp, &cam_up);
			mul_vec3f_by_scalar(&tmp, &tmp, move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
	}
// 	printf("campos:   %f %f %f\n", cam_pos.x, cam_pos.y, cam_pos.z);
// 	printf("camdir:   %f %f %f\n", cam_dir.x, cam_dir.y, cam_dir.z);
// 	printf("camup:    %f %f %f\n", cam_up.x, cam_up.y, cam_up.z);
	make_lookat_matrixf(lookat_matrix, &cam_pos, &cam_dir, &cam_up);
	recompute_gl_matrices_of_cam(current_camera());
}

static int last_mouse_x = -1, last_mouse_y = -1;

void standard_mouse_motion(int x, int y)
{
	static matrix4x4f xrot, yrot, rot, tmp;
	static vec3f x_axis, y_axis; 
	static bool first_time = true;
	if (first_time) {
		make_vec3f(&x_axis, 1, 0, 0);
		make_vec3f(&y_axis, 0, 1, 0);
		first_time = false;
	}
	matrix4x4f *lookat_matrix = lookat_matrix_of_cam(current_camera());
	float delta_x = x - last_mouse_x;	last_mouse_x = x;
	float delta_y = y - last_mouse_y;	last_mouse_y = y;
	float delta_factor = 0.002;
	make_rotation_matrix4x4f(&xrot, &x_axis, delta_factor * delta_y);
	make_rotation_matrix4x4f(&yrot, &y_axis, delta_factor * delta_x);
	multiply_matrices4x4f(&rot, &xrot, &yrot);
	multiply_matrices4x4f(&tmp, lookat_matrix, &rot);
	copy_matrix4x4f(lookat_matrix, &tmp);
	recompute_gl_matrices_of_cam(current_camera());
}

void standard_mouse_func(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		last_mouse_x = x, last_mouse_y = y;
}

void swap_buffers() {
	glutSwapBuffers();
}
void enter_glut_main_loop() {
	glutMainLoop();
}
