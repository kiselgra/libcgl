#ifdef WITH_GUILE

#include "scheme.h"
#include "gl-version.h"

#include <libguile.h>

void register_scheme_functions_for_shaders();
void register_scheme_functions_for_cameras();
void register_scheme_functions_for_textures();
void register_scheme_functions_for_glut();
void register_scheme_functions_for_meshes();
void register_scheme_functions_for_framebuffers();
void register_scheme_functions_for_impex();

void load_snarfed_definitions() {
	register_scheme_functions_for_shaders();
	register_scheme_functions_for_cameras();
	register_scheme_functions_for_textures();
	register_scheme_functions_for_framebuffers();
	register_scheme_functions_for_glut();
	register_scheme_functions_for_meshes();
	register_scheme_functions_for_impex();
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
	fprintf(stream, "(load \"" DATADIR "/scheme/reader-extensions.scm\")", filename);
	fprintf(stream, "(load \"" DATADIR "/scheme/shader.scm\")", filename);
	fprintf(stream, "(load \"" DATADIR "/scheme/vecnf.scm\")", filename);
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
	char *symb = scm_to_locale_string(scm_symbol_to_string(*(SCM*)s));
	#define case(X, Y) else if (strcmp(X, symb) == 0) return free(symb), Y
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
	case ("static-draw", GL_STATIC_DRAW);
	case ("triangles", GL_TRIANGLES);
	case ("triangle-strip", GL_TRIANGLE_STRIP);
	case ("lines", GL_LINES);
	case ("line-strip", GL_LINE_STRIP);
	case ("points", GL_POINTS);
	fprintf(stderr, "invalid schme symbol for glenum: %s.\n", symb);
	exit(-1);
}

vec3f list_to_vec3f(SCM v) {
	if (scm_is_false(scm_list_p(v)))     scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_2(v, scm_from_locale_string("is not a list")));
	if (scm_is_null(v))                  scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_1(scm_from_locale_string("the empty list is no valid vec3f")));
	if (scm_to_int(scm_length(v)) != 3)  scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_2(v, scm_from_locale_string("is not a three component list")));
	vec3f ret;
	SCM el;
	el = scm_list_ref(v, scm_from_int(0));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_2(el, scm_from_locale_string("(element 0) is not a real number")));
	ret.x = scm_to_double(el);
	el = scm_list_ref(v, scm_from_int(1));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_2(el, scm_from_locale_string("(element 1) is not a real number")));
	ret.y = scm_to_double(el);
	el = scm_list_ref(v, scm_from_int(2));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec3f-conversion"), scm_list_2(el, scm_from_locale_string("(element 2) is not a real number")));
	ret.z = scm_to_double(el);
	return ret;
}

vec4f list_to_vec4f(SCM v) {
	if (scm_is_false(scm_list_p(v)))     scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(v, scm_from_locale_string("is not a list")));
	if (scm_is_null(v))                  scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_1(scm_from_locale_string("the empty list is no valid vec4f")));
	if (scm_to_int(scm_length(v)) != 4)  scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(v, scm_from_locale_string("is not a three component list")));
	vec4f ret;
	SCM el;
	el = scm_list_ref(v, scm_from_int(0));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(el, scm_from_locale_string("(element 0) is not a real number")));
	ret.x = scm_to_double(el);
	el = scm_list_ref(v, scm_from_int(1));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(el, scm_from_locale_string("(element 1) is not a real number")));
	ret.y = scm_to_double(el);
	el = scm_list_ref(v, scm_from_int(2));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(el, scm_from_locale_string("(element 2) is not a real number")));
	ret.z = scm_to_double(el);
	el = scm_list_ref(v, scm_from_int(3));
	if (scm_is_false(scm_real_p(el)))
		scm_throw(scm_from_locale_symbol("vec4f-conversion"), scm_list_2(el, scm_from_locale_string("(element 3) is not a real number")));
	ret.w = scm_to_double(el);
	return ret;
}

SCM vec3f_to_list(vec3f *v) {
	return scm_list_3(scm_from_double(v->x), scm_from_double(v->y), scm_from_double(v->z));
}

SCM vec4f_to_list(vec4f *v) {
	return scm_list_4(scm_from_double(v->x), scm_from_double(v->y), scm_from_double(v->z), scm_from_double(v->w));
}


#endif

