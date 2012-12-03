#include "glut.h"
#include "camera.h"
#include "cgl-config.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>


#if CGL_GL == GL
	#include <GL/freeglut.h>
#elif CGL_GL == GLES
	#if LIBCGL_HAVE_EGLUT_X11 == 1
		#ifdef __cplusplus
		extern "C" {
		#endif
		#include <eglut.h>
		#ifdef __cplusplus
		}
		#endif
	#endif
#endif


#include <stdio.h>

static bool forward = true;
void please_use_a_backward_gl_context() {
	forward = false;
}

#if CGL_GL == GL || LIBCGL_HAVE_EGLUT_X11 == 1

void startup_glut(const char *title, int argc, char **argv, int gl_maj, int gl_min, int res_x, int res_y)
{
#if CGL_GL == GL
	glutInit(&argc, argv);
	glutInitContextVersion (gl_maj, gl_min);
	unsigned int flags = GLUT_DEBUG;
	if (forward) flags |= GLUT_FORWARD_COMPATIBLE;
	glutInitContextFlags(flags);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(res_x, res_y); 
	glutInitWindowPosition (100, 100);
	if (title) glutCreateWindow(title);
	else       glutCreateWindow("a gl window");
	
	glutMouseFunc(standard_mouse_func);
	glutMotionFunc(standard_mouse_motion);
	// glutReshapeFunc(reshape);
	// glutReshapeFunc(reshape);
	glutKeyboardFunc (standard_keyboard);
#elif LIBCGL_HAVE_EGLUT_X11
	eglutInitWindowSize (res_x, res_y); 
	eglutInitAPIMask(EGLUT_OPENGL_ES2_BIT);
	eglutInit(argc, argv);
// 	eglutInitContextVersion (gl_maj, gl_min);
// 	eglutInitWindowPosition (100, 100);
	if (title) eglutCreateWindow(title);
	else       eglutCreateWindow("a gl window");
	
	eglutMouseFunc(standard_mouse_func);
	eglutMotionFunc(standard_mouse_motion);
	// glutReshapeFunc(reshape);
	// glutReshapeFunc(reshape);
	eglutKeyboardFunc (standard_keyboard);
#endif
	glViewport(0,0,res_x,res_y);

}

#if CGL_GL_VERSION == GLES2
#define glutDisplayFunc eglutDisplayFunc
#define glutIdleFunc eglutIdleFunc
#define glutReshapeFunc eglutReshapeFunc
#define glutKeyboardFunc eglutKeyboardFunc
#define glutMotionFunc eglutMotionFunc
#define glutMainLoop eglutMainLoop
#define GLUT_DOWN EGLUT_DOWN
#define GLUT_UP EGLUT_UP
#define GLUT_LEFT_BUTTON EGLUT_LEFT_BUTTON
#endif

void register_display_function(     void (*fn)())                        { glutDisplayFunc(fn); }
void register_idle_function(        void (*fn)())                        { glutIdleFunc(fn); }
void register_keyboard_function(    void (*fn)(unsigned char, int, int)) { glutKeyboardFunc(fn); }
#if CGL_GL == GL
void register_keyboard_up_function( void (*fn)(unsigned char, int, int)) { glutKeyboardUpFunc(fn); }
void register_mouse_function(       void (*fn)(int, int, int, int))      { glutMouseFunc(fn); }
#endif
void register_mouse_motion_function(void (*fn)(int, int))                { glutMotionFunc(fn); }
void register_resize_function(      void (*fn)(int, int))                { glutReshapeFunc(fn); }

float cgl_cam_move_factor = 0.1;
void standard_keyboard(unsigned char key, int x, int y)
{
	vec3f tmp;
	vec3f cam_right, cam_pos, cam_dir, cam_up;
	matrix4x4f *lookat_matrix = lookat_matrix_of_cam(current_camera());
	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix);
	extract_dir_vec3f_of_matrix(&cam_dir, lookat_matrix);
	extract_up_vec3f_of_matrix(&cam_up, lookat_matrix);
	extract_right_vec3f_of_matrix(&cam_right, lookat_matrix);
	switch (key) {
		case 27:
			quit(0);
		case 's':
			copy_vec3f(&tmp, &cam_dir);
			mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'w':
			copy_vec3f(&tmp, &cam_dir);
			mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'a':
			copy_vec3f(&tmp, &cam_right);
			mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'd':
			copy_vec3f(&tmp, &cam_right);
			mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'f':
			copy_vec3f(&tmp, &cam_up);
			mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'r':
			copy_vec3f(&tmp, &cam_up);
			mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
			add_components_vec3f(&cam_pos, &cam_pos, &tmp);
			break;
		case 'p':
			printf("campos:   %f %f %f\n", cam_pos.x, cam_pos.y, cam_pos.z);
			printf("camdir:   %f %f %f\n", cam_dir.x, cam_dir.y, cam_dir.z);
			printf("camup:    %f %f %f\n", cam_up.x, cam_up.y, cam_up.z);
			break;
	}
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

void standard_resize_func(int w, int h) {
	glViewport(0,0,w,h);
	camera_ref cc = current_camera();
	if (valid_camera_ref(cc) && is_perspective_camera(current_camera()))
		change_projection_of_cam(current_camera(), camera_fovy(cc), h==0?1:(float)w/(float)h, camera_near(cc), camera_far(cc));
}

void swap_buffers() {
// eglut does so after drawing
#if CGL_GL == GL
	glutSwapBuffers();
#endif
}
void enter_glut_main_loop() {
	glutMainLoop();
}

#ifdef WITH_GUILE
#include <libguile.h>

SCM_DEFINE(s_set_move_factor, "set-move-factor!", 1, 0, 0, (SCM val), "") {
	if (!scm_is_number(val)) {
		fprintf(stderr, "provided value is not a number!\n");
		return SCM_BOOL_F;
	}
	double fact = scm_to_double(val);
	cgl_cam_move_factor = fact;
	return scm_from_double(fact);;
}

SCM_DEFINE(s_move_factor, "move-factor", 0, 0, 0, (), "") {
	return scm_from_double(cgl_cam_move_factor);
}

static SCM guile_display_handler_ref = SCM_BOOL_T;
static void guile_display_handler() {
	scm_call_0(guile_display_handler_ref);
}
static SCM guile_key_handler_ref = SCM_BOOL_T;
static void guile_key_handler(unsigned char c, int x, int y) {
	SCM ch = scm_integer_to_char(scm_from_int((int)c));
	SCM xx = scm_from_int(x);
	SCM yy = scm_from_int(y);
	scm_call_3(guile_key_handler_ref, ch, xx, yy);
}
static SCM guile_mouse_motion_handler_ref = SCM_BOOL_T;
static void guile_mouse_motion_handler(int x, int y) {
	SCM xx = scm_from_int(x);
	SCM yy = scm_from_int(y);
// 	printf("calling ugile motion handler %d %d\n", x, y);
	scm_call_2(guile_mouse_motion_handler_ref, xx, yy);
}
static SCM guile_mouse_handler_ref = SCM_BOOL_T;
static void guile_mouse_handler(int button, int state, int x, int y) {
        SCM b = scm_from_int(button);
        SCM s = scm_from_int(state);
	SCM xx = scm_from_int(x);
	SCM yy = scm_from_int(y);
	scm_call_4(guile_mouse_handler_ref, b, s, xx, yy);
}
static SCM guile_idle_handler_ref = SCM_BOOL_T;
static void guile_idle_handler() {
    scm_call_0(guile_idle_handler_ref);
}
SCM_DEFINE(s_register_display_function, "register-display-function", 1, 0, 0, (SCM handler), "") {
	SCM old = guile_display_handler_ref;
	guile_display_handler_ref = handler;
	register_display_function(guile_display_handler);
	return old;
}
SCM_DEFINE(s_register_key_function, "register-key-function", 1, 0, 0, (SCM handler), "") {
	SCM old = guile_key_handler_ref;
	guile_key_handler_ref = handler;
	register_keyboard_function(guile_key_handler);
	return old;
}
SCM_DEFINE(s_std_key_handler, "standard-key-function", 3, 0, 0, (SCM key, SCM x, SCM y), "") {
	char c = scm_to_int(scm_char_to_integer(key));
	int xx = scm_to_int(x);
	int yy = scm_to_int(y);
	standard_keyboard(c, xx, yy);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_reg_mouse_function, "register-mouse-function", 1, 0, 0, (SCM handler), "") {
	SCM old = guile_mouse_handler_ref;
	guile_mouse_handler_ref = handler;
	register_mouse_function(guile_mouse_handler);
	return old;
}
SCM_DEFINE(s_std_mouse_handler, "standard-mouse-function", 4, 0, 0, (SCM button, SCM state, SCM x, SCM y), "") {
	int b = scm_to_int(button);
	int s = scm_to_int(state);
	int xx = scm_to_int(x);
	int yy = scm_to_int(y);
	standard_mouse_func(b, s, xx, yy);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_std_mouse_motion_function, "register-mouse-motion-function", 1, 0, 0, (SCM handler), "") {
	SCM old = guile_mouse_motion_handler_ref;
	guile_mouse_motion_handler_ref = handler;
	register_mouse_motion_function(guile_mouse_motion_handler);
	return old;
}
SCM_DEFINE(s_std_mouse_motion_handler, "standard-mouse-motion-function", 2, 0, 0, (SCM x, SCM y), "") {
	int xx = scm_to_int(x);
	int yy = scm_to_int(y);
	standard_mouse_motion(xx, yy);
	return SCM_BOOL_T;
}
SCM_DEFINE(s_register_idle_function, "register-idle-function", 1, 0, 0, (SCM handler), "") {
    SCM old = guile_idle_handler_ref;
    if (scm_is_false(handler)) {
        register_idle_function(0);
    }
    else {
        guile_idle_handler_ref = handler;
        register_idle_function(guile_idle_handler);
    }
    return old;
}

SCM_DEFINE(s_swap_buffers, "glut:swap-buffers", 0, 0, 0, (), "") {
	swap_buffers();
	return SCM_BOOL_T;
}

SCM_DEFINE(s_post_redisplay, "glut:post-redisplay", 0, 0, 0, (), "") {
	glutPostRedisplay();
	return SCM_BOOL_T;
}

SCM_DEFINE(s_check_for_gl_errors, "check-for-gl-errors", 1, 0, 0, (SCM msg), "") {
    char *m = scm_to_locale_string(msg);
    check_for_gl_errors(m);
    free(m);
    return SCM_BOOL_T;
}

SCM_DEFINE(s_time_stamp, "glut:time-stamp", 0, 0, 0, (), "Wall time since glut started.") {
	return scm_from_int(glutGet(GLUT_ELAPSED_TIME));
}

void register_scheme_functions_for_glut() {
#ifndef SCM_MAGIC_SNARFER
#include "glut.x"
#endif
}

#endif

#endif
