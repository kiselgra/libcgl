#ifdef WITH_GUILE

#include "scheme.h"
#include "gl-version.h"

#include <libguile.h>

void register_scheme_functions_for_shaders();
void register_scheme_functions_for_cameras();
void register_scheme_functions_for_textures();
void register_scheme_functions_for_glut();

void load_snarfed_definitions() {
	register_scheme_functions_for_shaders();
	register_scheme_functions_for_cameras();
	register_scheme_functions_for_textures();
	register_scheme_functions_for_framebuffers();
	register_scheme_functions_for_glut();
}

void start_console_thread() {
	scm_c_eval_string("(define repl-thread (call-with-new-thread (lambda () "
	                                          "(use-modules (ice-9 readline)) "
											  "(activate-readline) "
											  "(top-repl) "
											  "(format #t \"THE GUILE REPL IS TERMINATING NOW.~%\"))))");
}




void load_configfile(const char *filename) {
	char *buffer = 0;
	size_t bs = 0;
	FILE *stream = open_memstream(&buffer, &bs);
	fprintf(stream, "(load \"%s\")", filename);
	fclose(stream);

	scm_c_eval_string(buffer);

	free(buffer);
}

unsigned int scheme_symbol_to_gl_enum(void *s) {
	if (!scm_is_symbol(*(SCM*)s)) {
		fprintf(stderr, "tried to get a glenum from a scheme non-symbol.\n");
		SCM port = scm_open_output_string();
		scm_display(*(SCM*)s, port);
		SCM s = scm_get_output_string(port);
		fprintf(stderr, "--> %s\n", scm_to_locale_string(s));
		exit(-1);
	}
	const char *symb = scm_to_locale_string(scm_symbol_to_string(*(SCM*)s));
	#define case(X, Y) else if (strcmp(X, symb) == 0) return Y
	if (0){}
	case ("tex2d", GL_TEXTURE_2D);
	case ("tex-2d", GL_TEXTURE_2D);
	case ("texture-2d", GL_TEXTURE_2D);
	case ("float", GL_FLOAT);
	case ("unsigned-byte", GL_UNSIGNED_BYTE);
	case ("rgb", GL_RGB);
	case ("rgba", GL_RGBA);
	case ("depth-component", GL_DEPTH_COMPONENT);
#if CGL_GL_VERSION == GL3
	case ("rgb-8", GL_RGB8);
	case ("rgba-8", GL_RGBA8);
	case ("rgba-32f", GL_RGBA32F);
	case ("depth-component-32f", GL_DEPTH_COMPONENT32F);
#endif
	case ("nearest", GL_NEAREST);
	case ("repeat", GL_REPEAT);
	fprintf(stderr, "invalid schme symbol for glenum: %s.\n", symb);
	exit(-1);
}

#endif

