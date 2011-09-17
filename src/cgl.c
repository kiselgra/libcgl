#include "cgl.h"

#include "glut.h"

#include <GL/glew.h>
#include <GL/glxew.h>
#include <stdio.h>
#include <stdlib.h>

void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, bool verbose) {
	startup_glut(window_title, argc, argv, gl_major, gl_minor, res_x, res_y);
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	ignore_gl_error("glew-init");
	
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	if (verbose) {
		dump_gl_info();
		printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}
}

// actually, this should not be necessary as soon as we use the debugging extension...

static error_handler_t error_handler = standard_error_handler;

void register_error_handler(error_handler_t h) {
	error_handler = h;
}

void check_for_gl_errors(const char *where)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		error_handler(error, where);
}

void standard_error_handler(GLenum error, const char *where) {
	fprintf(stderr, "GL error %d detected in %s\n", error, where);
	exit(-1);
}

void ignore_gl_error(const char *function)
{
	GLenum error = glGetError();
	fprintf(stderr, "Ignoring GL error %s (for %s).\n", gluErrorString(error), function);
}

void dump_gl_info(void)
{
	check_for_gl_errors("pre dump-info");
	printf("Vendor: %s\n", glGetString (GL_VENDOR));
	printf("Renderer: %s\n", glGetString (GL_RENDERER));
	printf("Version: %s\n", glGetString (GL_VERSION));
	printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	check_for_gl_errors("dump-info");
}


