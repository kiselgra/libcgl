#ifndef __CGL_GL_DEBUG_H__ 
#define __CGL_GL_DEBUG_H__ 

#include "gl-version.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*gl_debug_function)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* user_param);


void default_debug_function(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* user_param);
void start_debug_output();
void stop_debug_output();
const char* debug_code_to_string(GLenum code);


#ifdef __cplusplus
}
#endif

#endif

