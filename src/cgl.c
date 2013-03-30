#include "cgl.h"

#include "glut.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <termios.h>	// tc[sg]etattr
#include <unistd.h>		// tc[sg]etattr

#ifdef WITH_GUILE
#include <libguile.h>
#include "scheme.h"
#endif

/*! \defgroup administrative Administrative Functions
 *  Startup, error handling, and such.
 */

extern void load_internal_configfiles(void);

static void hop(void *data, int argc, char **argv) {
#ifdef WITH_GUILE
	load_snarfed_definitions();
	load_internal_configfiles();
	if (argv[0]) load_configfile(argv[0]);
	start_console_thread();
#endif

	((void(*)())data)();	// run the user supplied 'inner main'
}

static void* cfg_only(void *data) {
#ifdef WITH_GUILE
	load_snarfed_definitions();
	load_internal_configfiles();
	if (data) load_configfile((char*)data);
#endif
	return 0;
}

//! This is to return to a sane terminal state after (potentially) quitting guile in some tasteles way.
static struct termios termios;

/*! \brief Startup Code. Will call \c call (we often speak of it as `actual_main').
 *  \ingroup administrative
 *  
 *  Hands control to cgl, which will start up, create a window, setup gl, possibly start guile, possible read the config file (if not 0) and finally call \c call.
 */
void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, void (*call)(), int use_guile, bool verbose, const char *initfile) {
	
	if (isatty(STDOUT_FILENO))
		tcgetattr(STDOUT_FILENO, &termios);
#if LIBCGL_HAVE_LIBGLUT == 1
	startup_glut(window_title, argc, argv, gl_major, gl_minor, res_x, res_y);
#endif
	
#if CGL_GL == GL
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
#endif

	start_debug_output();

#ifdef WITH_GUILE
	if (use_guile == with_guile) {
		char *p[2] = { (char*)initfile, 0 };
		scm_boot_guile(initfile?1:0, p, hop, (void*)call);
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

static gl_error_handler_t error_handler = standard_gl_error_handler;

/*! \brief Use this to register an error handler called upon detection an error by means of \ref check_for_gl_errors.
 *  \ingroup administrative
 */
void register_gl_error_handler(gl_error_handler_t h) {
	error_handler = h;
}

/*! \brief Checks to see if the GL is in an error state and calls the registered error hander (see \ref register_gl_error_handler).
 *  This might have happend anywhere between two calls to this function.
 *  \ingroup administrative
 *  \note This is mostly unnecessary when the debugging extension is working (which it should be).
 */
void check_for_gl_errors(const char *where)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		error_handler(error, where);
}

void standard_gl_error_handler(GLenum error, const char *where) {
    if (error == GL_INVALID_ENUM)
        fprintf(stderr, "GL error 'Invalid Enum' (%d) detected in %s\n", error, where);
    else if (error == GL_INVALID_VALUE)
       fprintf(stderr, "GL error 'Invalid Value' (%d) detected in %s\n", error, where);
    else if (error == GL_INVALID_OPERATION)
       fprintf(stderr, "GL error 'Invalid Operation' (%d) detected in %s\n", error, where);
    else
        fprintf(stderr, "GL error %d detected in %s\n", error, where);
	exit(-1);
}

void ignore_gl_error(const char *function)
{
	GLenum error = glGetError();
#if CGL_GL == GL
	fprintf(stderr, "Ignoring GL error %s (for %s).\n", gluErrorString(error), function);
#else
	fprintf(stderr, "Ignoring GL error %d (for %s) (is glu supported here?).\n", error, function);
#endif
}

static error_message_handler_t error_message_handler = standard_error_message_handler;
void standard_error_message_handler(const char *fmt, va_list ap) {
	vfprintf(stderr, fmt, ap);
}
void register_error_message_handler(error_message_handler_t h) {
	error_message_handler = h;
}
void print_error_message(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	error_message_handler(fmt, ap);
	va_end(ap);
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
	if (isatty(STDOUT_FILENO))
		tcsetattr(STDOUT_FILENO, TCSANOW, &termios);
#ifdef WITH_GUILE
	scm_c_eval_string("(cancel-thread repl-thread)");
	scm_c_eval_string("(join-thread repl-thread)");
	scm_primitive_exit(scm_from_int(status));
#endif
	exit(status);
}


char* gl_enum_string(GLenum e) {
#define c(X) case X: return #X;
	switch (e) {
	// textures
	c(GL_TEXTURE_2D)
	c(GL_RGB)
	c(GL_RGBA)
	c(GL_DEPTH_COMPONENT)
#if CGL == GL
	c(GL_RGB8)
	c(GL_RGBA8)
	c(GL_DEPTH_COMPONENT24)
	c(GL_DEPTH_COMPONENT32)
	c(GL_DEPTH_COMPONENT32F)
#endif
	c(GL_FLOAT)
	c(GL_FRAMEBUFFER_COMPLETE)
	c(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	c(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
#if CGL_GL == GL
	c(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
	c(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
#endif
	c(GL_FRAMEBUFFER_UNSUPPORTED)
	}
	return "not handled yet";
}

