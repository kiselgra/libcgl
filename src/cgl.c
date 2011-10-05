#include "cgl.h"

#include "glut.h"
#include "scheme.h"

#include <GL/glew.h>
#include <GL/glxew.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_GUILE
#include <libguile.h>
#endif

static void hop(void *data, int argc, char **argv) {
	load_snarfed_definitions();
	load_configfile(argv[0]);
	start_console_thread();

	((void(*)())data)();	// run the user supplied 'inner main'
}

static void cfg_only(void *data) {
	load_snarfed_definitions();
	load_configfile((char*)data);
}

void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, void (*call)(), int use_guile, bool verbose, const char *initfile) {
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

#ifdef WITH_GUILE
	if (use_guile == with_guile) {
		char *p[2] = { (char*)initfile, 0 };
		scm_boot_guile(1, p, hop, (void*)call);
	}
	else if (use_guile == guile_cfg_only) {
		scm_with_guile(cfg_only, (char*)initfile);
		call();
	}
	else
		call();
#else
	call();
#endif
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

void quit(int status) {
	scm_c_eval_string("(cancel-thread repl-thread)");
	scm_c_eval_string("(join-thread repl-thread)");
	exit(status);
}
