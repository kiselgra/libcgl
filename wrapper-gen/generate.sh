#/bin/bash
defines="-D GL_ATI_blend_equation_separate=0 -D GL_GLEXT_PROTOTYPES -D GL_ARB_cl_event=0 -D GL_ARB_debug_output=0 -D GL_AMD_debug_output=0 -D GL_NV_half_float=0 -D GL_NV_vdpau_interop=0"
# sed /usr/include/GL/glext.h -e 's:#\s*include:// would include:g' | cpp $defines > work.gl.h
sed gl3.h -e 's:#\s*include:// would include:g' | cpp $defines > work.gl.h
sed -ie 's/__attribute__((visibility("default")))//' work.gl.h
sed -ie 's/GL_APICALL//'  work.gl.h
sed -ie 's/GL_APIENTRY//' work.gl.h
sed -ie 's/typedef .*;//' work.gl.h
sed -ie '/^\s*$/d' work.gl.h
sbcl --noinform --load parse.lisp --eval '(quit)' work.gl.h gl.wrappers.c
#guile-snarf -o gl.wrappers.c.x $(pkg-config --cflags guile-2.0) gl.wrappers.c
