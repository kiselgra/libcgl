#ifndef __SCHEME_H__ 
#define __SCHEME_H__ 

void load_snarfed_definitions();
void start_console_thread();
void load_configfile(const char *filename);

unsigned int scheme_symbol_to_gl_enum(void *s);

#endif

