#ifndef __CGL_H__ 
#define __CGL_H__ 

#include <stdbool.h>
#include <GL/glew.h>

void startup_cgl(const char *window_title, int gl_major, int gl_minor, int argc, char **argv, int res_x, int res_y, bool verbose);

// error handling. the gl debuggin extension should replace this...

void check_for_gl_errors(const char *function);
void standard_error_handler(GLenum error, const char *where);
void ignore_gl_error(const char *function);
void dump_gl_info(void);

typedef void (*error_handler_t)(GLenum error, const char *where);
void register_error_handler(error_handler_t h);

#endif

