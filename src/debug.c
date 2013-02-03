#include "debug.h"

#include "gl-version.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef GL_KHR_debug
#define HAVE_GL_DEBUGGING 1
#else
#define HAVE_GL_DEBUGGING 0
#endif

const char* debug_code_to_short_string(GLenum code);

void critical_debug_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* user_param) {
	printf("critical condition, quitting.\n");
	exit(-1);
}

void default_debug_function(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* user_param) {
	printf("callback\n");
	printf("Type: %s, Source: %s, ID: %d, Severity: %s\n",
	       debug_code_to_short_string(type),
	       debug_code_to_short_string(source),id,
	       debug_code_to_short_string(severity));
	printf("Message: %s\n ", message);
	if (type == GL_DEBUG_TYPE_ERROR && source == GL_DEBUG_SOURCE_API && severity == GL_DEBUG_SEVERITY_HIGH) {
		critical_debug_message(source, type, id, severity, length, message, user_param);
	}
}


void start_debug_output() {
#if HAVE_GL_DEBUGGING == 1
	glDebugMessageCallback(default_debug_function, 0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	if (!glewIsSupported("GL_ARB_debug_output"))
		fprintf(stderr, "GLEW does not support debugging!\n");

	int f;
    glGetIntegerv(GL_CONTEXT_FLAGS, &f);
    if ((f & GL_CONTEXT_FLAG_DEBUG_BIT) == 0)
		fprintf(stderr, "GL context is no debugging context!\n");

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 1, GL_DEBUG_SEVERITY_HIGH, 4, "test");
#else
	fprintf(stderr, "GL debugging is not supported (or I don't know about).\n");
#endif
}

void stop_debug_output() {
#if HAVE_GL_DEBUGGING == 1
	glDisable(GL_DEBUG_OUTPUT);
#else
	fprintf(stderr, "GL debugging is not supported (or I don't know about).\n");
#endif
}

//! directly taken (and transformed) from KHR_debug extension text.
const char* debug_code_to_string(GLenum code) {
	switch (code) {
#if HAVE_GL_DEBUGGING == 1
	// Tokens accepted or provided by the <source> parameters of
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC, and the <sources> 
	// parameter of GetDebugMessageLog:
	
	case GL_DEBUG_SOURCE_API:             return "GL_DEBUG_SOURCE_API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     return "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     return "GL_DEBUG_SOURCE_APPLICATION"; break;
	case GL_DEBUG_SOURCE_OTHER:           return "GL_DEBUG_SOURCE_OTHER"; break;
	
	// Tokens accepted or provided by the <type> parameters of
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC, and the <types> 
	// parameter of GetDebugMessageLog:
	
	case GL_DEBUG_TYPE_ERROR:               return "GL_DEBUG_TYPE_ERROR"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         return "GL_DEBUG_TYPE_PORTABILITY"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         return "GL_DEBUG_TYPE_PERFORMANCE"; break;
	case GL_DEBUG_TYPE_OTHER:               return "GL_DEBUG_TYPE_OTHER"; break;
	case GL_DEBUG_TYPE_MARKER:              return "GL_DEBUG_TYPE_MARKER"; break;
	
	// Tokens accepted or provided by the <severity> parameters of
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC callback functions, 
	// and the <severities> parameter of GetDebugMessageLog:
	
	case GL_DEBUG_SEVERITY_HIGH:         return "GL_DEBUG_SEVERITY_HIGH"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       return "GL_DEBUG_SEVERITY_MEDIUM"; break;
	case GL_DEBUG_SEVERITY_LOW:          return "GL_DEBUG_SEVERITY_LOW"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "GL_DEBUG_SEVERITY_NOTIFICATION"; break;
#endif
	default:
		return "**** <<<< unknown code >>>> ****";
	}
}

const char* debug_code_to_short_string(GLenum code) {
	switch (code) {
	// Tokens accepted or provided by the <source> parameters of
#if HAVE_GL_DEBUGGING == 1
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC, and the <sources> 
	// parameter of GetDebugMessageLog:
	
	case GL_DEBUG_SOURCE_API:             return "api"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "window-system"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader-compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     return "third-party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     return "application"; break;
	case GL_DEBUG_SOURCE_OTHER:           return "other"; break;
	
	// Tokens accepted or provided by the <type> parameters of
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC, and the <types> 
	// parameter of GetDebugMessageLog:
	
	case GL_DEBUG_TYPE_ERROR:               return "error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated-behavior"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "undefined-behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         return "portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         return "performance"; break;
	case GL_DEBUG_TYPE_OTHER:               return "other"; break;
	case GL_DEBUG_TYPE_MARKER:              return "marker"; break;
	
	// Tokens accepted or provided by the <severity> parameters of
	// DebugMessageControl, DebugMessageInsert and DEBUGPROC callback functions, 
	// and the <severities> parameter of GetDebugMessageLog:
	
	case GL_DEBUG_SEVERITY_HIGH:         return "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       return "medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          return "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification"; break;
#endif
	default:
		return "**** <<<< unknown code >>>> ****";
	}
}

