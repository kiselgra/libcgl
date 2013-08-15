#ifndef __ATOMIC_BUFFER_H__ 
#define __ATOMIC_BUFFER_H__ 

#include "cgl.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int id;
} atomic_buffer_ref;


atomic_buffer_ref make_atomic_buffer(const char *name, unsigned int w, unsigned int h);
void bind_atomic_buffer(atomic_buffer_ref ref, unsigned int location);
void unbind_atomic_buffer(atomic_buffer_ref ref, unsigned int location);
int* map_atomic_int_buffer(atomic_buffer_ref ref, GLenum how);
unsigned int* map_atomic_unsigned_int_buffer(atomic_buffer_ref ref, GLenum how);
void unmap_atomic_buffer(atomic_buffer_ref ref);
void reset_atomic_buffer(atomic_buffer_ref ref, int value);
void reset_atomic_bufferu(atomic_buffer_ref ref, unsigned int value);

atomic_buffer_ref find_atomic_buffer(const char *name);
char* atomic_buffer_name(atomic_buffer_ref ref);

define_array(atomic_buffer);

#ifdef __cplusplus
}
#endif

#endif

