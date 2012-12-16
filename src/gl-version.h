#ifndef __GL_HEADER_H__ 
#define __GL_HEADER_H__ 

#define GL3 0x7c03
#define GL4 0x7c04
#define GLES2 0x7c51302
#define GL 0x7c
#define GLES0x7c513

#if CGL_GL_VERSION == GL3 || CGL_GL_VERSION == GL4
#define CGL_GL GL
#include <GL/glew.h>
#include <GL/glxew.h>
#elif CGL_GL_VERSION == GLES2
#define CGL_GL GLES
#ifdef __IPHONE_5_0
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#else
#error No valid GL version specified in CGL_GL_VERSION!
#endif

#endif

