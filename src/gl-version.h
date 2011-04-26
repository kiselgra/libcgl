#ifndef __GL_HEADER_H__ 
#define __GL_HEADER_H__ 

#define GL3 0x7c03
#define GLES2 0x7c51302

#if CGL_GL_VERSION == GL3
#include <GL/glew.h>
#include <GL/glxew.h>
#elif CGL_GL_VERSION == GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#error No valid GL version specified in CGL_GL_VERSION!
#endif

#endif

