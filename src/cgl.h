#ifndef __CGL_H__ 
#define __CGL_H__ 

// #include <libguile.h>

#include <stdbool.h>
#include <GL/glew.h>

enum { invalid_ref = -1 };
enum { without_guile, with_guile, guile_cfg_only };

// initfile may be 0.
void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, void (*call)(), int use_guile, bool verbose, const char *initfile);

// error handling. the gl debuggin extension should replace this...

void check_for_gl_errors(const char *function);
void standard_error_handler(GLenum error, const char *where);
void ignore_gl_error(const char *function);
void dump_gl_info(void);

void quit(int status);

typedef void (*error_handler_t)(GLenum error, const char *where);
void register_error_handler(error_handler_t h);

char* gl_enum_string(GLenum e);

#endif

