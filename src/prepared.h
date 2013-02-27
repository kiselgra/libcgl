#ifndef __CGL_PREPARED_H__ 
#define __CGL_PREPARED_H__ 

/* 
 * stuff prepared for you, but not actually on the same
 * design-level as cgl, meaning not providing infrastructure,
 * but implementing small parts of a real graphics system.
 *
 */

#include <libmcm-0.0.1/matrix.h>

#include "mesh.h"
#include "shader.h"


#ifdef __cplusplus
extern "C" {
#endif


/*! \addtogroup meshgen Built-in Meshes
 *
 * For all generated meshes, the order the vertex data is added to the vbo is
 * position, normal, texture coordinate
 * (as appropriate)
 *
 * @{
 */

//! Returns a quad mesh (two triangles), originally spanning [(-1,-1,0) , (1,1,0)], transformed by trafo (which may be 0).
mesh_ref make_quad(const char *name, matrix4x4f *trafo);

//! Same as \ref make_quad; the normal is computed
mesh_ref make_quad_with_normal(const char *name, matrix4x4f *trafo); //! \attention UNTESTED!

//! Same as \ref make_quad but with tex coords [(0,0),(1,1)] (where the tcs are not transformed)
mesh_ref make_quad_with_tc(const char *name, matrix4x4f *trafo);

//! \ref make_quad_with_normal + \ref make_quad_with_tc
mesh_ref make_quad_with_normal_and_tc(const char *name, matrix4x4f *trafo); //! \attention UNTESTED!

mesh_ref make_circle(const char *name, int n, matrix4x4f *trafo);

//! Returns a cube mesh [-1,-1,-1]x[1,1,1]
mesh_ref make_cube(const char *name, matrix4x4f *trafo);

//! @}

/* 
 * tangent space
 */

float triangle_area(const vec3f *a, const vec3f *b, const vec3f *c);

vec4f* generate_tangent_space_from_tri_mesh(const vec3f *vertex, const vec3f *normal, const vec2f *tc, unsigned int vertex_data_len, const unsigned int *index, unsigned int indices);



/*! \brief shader convenience if you have no scheme at hand
 *  \ingroup shaders
 */
shader_ref make_shader_from_strings(const char *name, const char *vert, const char *frag, int number_of_inputs, ...);

#ifdef __cplusplus
}
#endif


#endif

