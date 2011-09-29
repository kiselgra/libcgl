#ifdef WITH_GUILE

#include "scheme.h"

#include <libguile.h>

void register_scheme_functions_for_shaders();
void register_scheme_functions_for_cameras();
void register_scheme_functions_for_textures();

void load_snarfed_definitions() {
	register_scheme_functions_for_shaders();
	register_scheme_functions_for_cameras();
	register_scheme_functions_for_textures();
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

#endif

