#ifndef __CGL_H__ 
#define __CGL_H__ 

#include "cgl-config.h"
#include "gl-version.h"
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


enum { invalid_ref = -1 };
enum { without_guile, with_guile, guile_cfg_only };

// initfile may be 0.
void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, void (*call)(), int use_guile, bool verbose, const char *initfile);
void please_use_a_backward_gl_context();

// error handling. the gl debuggin extension should replace this...

void check_for_gl_errors(const char *function);
void standard_gl_error_handler(GLenum error, const char *where);
void ignore_gl_error(const char *function);
void dump_gl_info(void);

void quit(int status);

typedef void (*gl_error_handler_t)(GLenum error, const char *where);
void register_gl_error_handler(gl_error_handler_t h);

char* gl_enum_string(GLenum e);

typedef void (*error_message_handler_t)(const char *fmt, va_list ap);
void standard_error_message_handler(const char *fmt, va_list ap);
void register_error_message_handler(error_message_handler_t);
void print_error_message(const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif

