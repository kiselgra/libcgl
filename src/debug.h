#ifndef __CGL_GL_DEBUG_H__ 
#define __CGL_GL_DEBUG_H__ 

#include "gl-version.h"

typedef void (*gl_debug_function)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);


void default_debug_function(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
void start_debugging();
const char* debug_code_to_string(GLenum code);

#endif

