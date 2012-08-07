#include "prepared.h"

#include <list>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {

mesh_ref make_quad(const char *name, matrix4x4f *trafo) {
	vec3f v[] = { {-1, -1, 0},
	              {-1,  1, 0},
				  { 1,  1, 0},
				  { 1,  1, 0},
				  { 1, -1, 0},
				  {-1, -1, 0} };
	if (trafo)
		for (int i = 0; i < 6; ++i) {
			vec4f cur = { v[i].x, v[i].y, v[i].z, 1 };
			vec4f res;
			multiply_matrix4x4f_vec4f(&res, trafo, &cur);
			v[i].x = res.x; v[i].y = res.y; v[i].z = res.z;
		}
	mesh_ref mesh = make_mesh(name, 1);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vt", GL_FLOAT, 6, 3, v, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;
}

mesh_ref make_quad_with_tc(const char *name, matrix4x4f *trafo) {
	vec3f v[] = { {-1, -1, 0},
	              {-1,  1, 0},
				  { 1,  1, 0},
				  { 1,  1, 0},
				  { 1, -1, 0},
				  {-1, -1, 0} };
	vec2f t[] = { {0, 0},
	              {0, 1},
				  {1, 1},
				  {1, 1},
				  {1, 0},
				  {0, 0} };
	if (trafo)
		for (int i = 0; i < 6; ++i) {
			vec4f cur = { v[i].x, v[i].y, v[i].z, 1 };
			vec4f res;
			multiply_matrix4x4f_vec4f(&res, trafo, &cur);
			v[i].x = res.x; v[i].y = res.y; v[i].z = res.z;
		}
	mesh_ref mesh = make_mesh(name, 2);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vt", GL_FLOAT, 6, 3, v, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "tc", GL_FLOAT, 6, 2, t, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;
}

mesh_ref make_quad_with_normal(const char *name, matrix4x4f *trafo) {
	vec3f v[] = { {-1, -1, 0},
	              {-1,  1, 0},
				  { 1,  1, 0},
				  { 1,  1, 0},
				  { 1, -1, 0},
				  {-1, -1, 0} };
	vec3f n[] = { { 0,  0, 1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1} };
	if (trafo) {
		matrix4x4f tmp, norm;
		invert_matrix4x4f(&tmp, trafo);
		transpose_matrix4x4f(&norm, &tmp);
		for (int i = 0; i < 6; ++i) {
			// transform v
			vec4f cur = { v[i].x, v[i].y, v[i].z, 1 };
			vec4f res;
			multiply_matrix4x4f_vec4f(&res, trafo, &cur);
			v[i].x = res.x; v[i].y = res.y; v[i].z = res.z;
			// transform n
			cur.x = n[i].x; cur.y = n[i].y; cur.z = n[i].z; cur.w = 0;
			multiply_matrix4x4f_vec4f(&res, &norm, &cur);
			n[i].x = res.x; n[i].y = res.y; n[i].z = res.z;
		}
	}
	mesh_ref mesh = make_mesh(name, 2);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vt", GL_FLOAT, 6, 3, v, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "vn", GL_FLOAT, 6, 3, n, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;
}

mesh_ref make_quad_with_normal_and_tc(const char *name, matrix4x4f *trafo) {
	vec3f v[] = { {-1, -1, 0},
	              {-1,  1, 0},
				  { 1,  1, 0},
				  { 1,  1, 0},
				  { 1, -1, 0},
				  {-1, -1, 0} };
	vec2f t[] = { {0, 0},
	              {0, 1},
				  {1, 1},
				  {1, 1},
				  {1, 0},
				  {0, 0} };
	vec3f n[] = { { 0,  0, 1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1} };
	if (trafo) {
		matrix4x4f tmp, norm;
		invert_matrix4x4f(&tmp, trafo);
		transpose_matrix4x4f(&norm, &tmp);
		for (int i = 0; i < 6; ++i) {
			// transform v
			vec4f cur = { v[i].x, v[i].y, v[i].z, 1 };
			vec4f res;
			multiply_matrix4x4f_vec4f(&res, trafo, &cur);
			v[i].x = res.x; v[i].y = res.y; v[i].z = res.z;
			// transform n
			cur.x = n[i].x; cur.y = n[i].y; cur.z = n[i].z; cur.w = 0;
			multiply_matrix4x4f_vec4f(&res, &norm, &cur);
			n[i].x = res.x; n[i].y = res.y; n[i].z = res.z;
		}
	}
	mesh_ref mesh = make_mesh(name, 3);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vt", GL_FLOAT, 6, 3, v, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "vn", GL_FLOAT, 6, 3, n, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "tc", GL_FLOAT, 6, 2, t, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;
}

mesh_ref make_cube(const char *name, matrix4x4f *trafo) {
	vec3f v[] = { {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, { 1,  1,  1}, {-1,  1,  1}, {-1, -1,  1}, // FRONT
	              {-1, -1, -1}, {-1,  1, -1}, { 1,  1, -1}, { 1,  1, -1}, { 1, -1, -1}, {-1, -1, -1}, // BACK
	              {-1, -1,  1}, {-1,  1,  1}, {-1,  1, -1}, {-1,  1, -1}, {-1, -1, -1}, {-1, -1,  1}, // LEFT
	              { 1, -1, -1}, { 1,  1, -1}, { 1,  1,  1}, { 1,  1,  1}, { 1, -1,  1}, { 1, -1, -1}, // RIGHT
	              {-1,  1,  1}, { 1,  1,  1}, { 1,  1, -1}, { 1,  1, -1}, {-1,  1, -1}, {-1,  1,  1}, // TOP
	              {-1, -1,  1}, {-1, -1, -1}, { 1, -1, -1}, { 1, -1, -1}, { 1, -1,  1}, {-1, -1,  1}, // BOTTOM
	};
	vec3f n[] = { { 0, 0, 1 },  { 0, 0, 1 },  { 0, 0, 1 },  { 0, 0, 1 },  { 0, 0, 1 },  { 0, 0, 1 }, 
	              { 0, 0,-1 },  { 0, 0,-1 },  { 0, 0,-1 },  { 0, 0,-1 },  { 0, 0,-1 },  { 0, 0,-1 },
	              {-1, 0, 0 },  {-1, 0, 0 },  {-1, 0, 0 },  {-1, 0, 0 },  {-1, 0, 0 },  {-1, 0, 0 },
	              { 1, 0, 0 },  { 1, 0, 0 },  { 1, 0, 0 },  { 1, 0, 0 },  { 1, 0, 0 },  { 1, 0, 0 },
	              { 0, 1, 0 },  { 0, 1, 0 },  { 0, 1, 0 },  { 0, 1, 0 },  { 0, 1, 0 },  { 0, 1, 0 },
	              { 0,-1, 0 },  { 0,-1, 0 },  { 0,-1, 0 },  { 0,-1, 0 },  { 0,-1, 0 },  { 0,-1, 0 } 
	};
	vec2f t[] = { {0,0},    {1,0},    {1,1},    {1,1},    {0,1},    {0,0}, 
	              {0,0},    {0,1},    {1,1},    {1,1},    {1,0},    {0,0}, 
	              {0,1},    {1,1},    {1,0},    {1,0},    {0,0},    {0,1}, 
	              {0,0},    {1,0},    {1,1},    {1,1},    {0,1},    {0,0}, 
	              {0,1},    {1,1},    {1,0},    {1,0},    {0,0},    {0,1}, 
				  {0,1},    {0,0},    {1,0},    {1,0},    {1,1},    {0,1}, 
	};
	mesh_ref mesh = make_mesh(name, 3);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "vt", GL_FLOAT, 36, 3, v, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "vn", GL_FLOAT, 36, 3, n, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "tc", GL_FLOAT, 36, 2, t, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	return mesh;

}




/*
 *   tangent space stuff
 */


float triangle_area(const vec3f *a, const vec3f *b, const vec3f *c) {
        vec3f ab, ac, cross;
        sub_components_vec3f(&ab, b, a);
        sub_components_vec3f(&ac, c, a);
        cross_vec3f(&cross, &ab, &ac);
        return 0.5 * dot_vec3f(&cross, &cross);
}

vec4f* generate_tangent_space_from_tri_mesh(const vec3f *vertex, const vec3f *normal, const vec2f *tc, unsigned int vertex_data_len, const unsigned int *index, unsigned int indices) {
        if (indices % 3 != 0) return 0;
        unsigned int triangles = indices/3;
        struct tb {
                vec3f t, b;
        };
        tb *per_triangle = new tb[triangles];
        std::list<int> *per_vertex = new std::list<int>[vertex_data_len];
        for (int i = 0; i < triangles; ++i) {
                int vi1 = index[3*i], vi2 = index[3*i+1], vi3 = index[3*i+2];
                int ti1 = index[3*i], ti2 = index[3*i+1], ti3 = index[3*i+2];
                vec3f q1, q2;
                sub_components_vec3f(&q1, vertex+vi2, vertex+vi1);
                sub_components_vec3f(&q2, vertex+vi3, vertex+vi1);
                vec2f uv1, uv2;
                sub_components_vec2f(&uv1, tc+ti2, tc+ti1);
                sub_components_vec2f(&uv2, tc+ti3, tc+ti1);

                vec2f inv_t_row = { uv2.y, -uv1.y },
                          inv_s_row = { -uv2.x, uv1.x };
                vec2f qx_col = { q1.x, q2.x },
                          qy_col = { q1.y, q2.y },
                          qz_col = { q1.z, q2.z };

                vec3f t = { dot_vec2f(&inv_t_row, &qx_col),
                            dot_vec2f(&inv_t_row, &qy_col),
                            dot_vec2f(&inv_t_row, &qz_col) },
                          b = { dot_vec2f(&inv_s_row, &qx_col),
                            dot_vec2f(&inv_s_row, &qy_col),
                            dot_vec2f(&inv_s_row, &qz_col) };

                float normalize = uv1.x*uv2.y - uv2.x*uv1.y;
                div_vec3f_by_scalar(&t, &t, normalize);
                div_vec3f_by_scalar(&b, &b, normalize);

                per_triangle[i].t = t;
                per_triangle[i].b = b;

                per_vertex[vi1].push_back(i);  // register the triangle with all vertices for back reference.
                per_vertex[vi2].push_back(i);
                per_vertex[vi3].push_back(i);
        }
        vec4f *tangents = new vec4f[vertex_data_len];
        for (int i = 0; i < vertex_data_len; ++i) {
                float area = 0.0f;
                vec3f accum_t = {0,0,0};
                vec3f accum_b = {0,0,0};
                vec4f result = {0,0,0,0};
                for (std::list<int>::iterator it = per_vertex[i].begin(); it != per_vertex[i].end(); ++it) {
                        int tri = 3**it;
                        float a = triangle_area(vertex+index[tri+0], vertex+index[tri+1], vertex+index[tri+2]);
                        vec3f t, b;
                        mul_vec3f_by_scalar(&t, &per_triangle[*it].t, a);
                        mul_vec3f_by_scalar(&b, &per_triangle[*it].b, a);
                        add_components_vec3f(&accum_t, &accum_t, &t);
                        add_components_vec3f(&accum_b, &accum_b, &b);
                        area += a;
                }
                if (area > 0) {
                        div_vec3f_by_scalar(&accum_t, &accum_t, area);
                        div_vec3f_by_scalar(&accum_b, &accum_b, area);
                        
						/* not required, not tested, included for the sake of completeness
						float dot = dot_vec3f(&accum_t, &accum_b);
                        result.x = accum_t.x;
                        result.y = accum_t.y;
                        result.z = accum_t.z;
                        result.w = (dot < 0 ? -1 : 1);
						*/

                        vec3f newt, newb, tmp, tmp2; 
                        const vec3f *norm = normal+i;
                        mul_vec3f_by_scalar(&tmp, norm, dot_vec3f(norm, &accum_t));
                        sub_components_vec3f(&newt, &accum_t, &tmp);
                        normalize_vec3f(&newt);

                        cross_vec3f(&tmp, norm, &newt);
                        float dot = dot_vec3f(&tmp, &accum_b);
                        result.x = newt.x;
                        result.y = newt.y;
                        result.z = newt.z;
                        result.w = (dot < 0 ? -1 : 1);
                }
                tangents[i] = result;
        }

        delete [] per_triangle;
        delete [] per_vertex;

        return tangents;
}


shader_ref make_shader_from_strings(const char *name, const char *vert, const char *frag, int number_of_inputs, ...) {
	va_list ap;
	const char **input_name = new const char*[number_of_inputs];
	va_start(ap, number_of_inputs);
	for (int i = 0; i < number_of_inputs; ++i)
		input_name[i] = va_arg(ap, char*);
	va_end(ap);
	
	shader_ref shader = make_shader(name, number_of_inputs, 0);
	add_vertex_source(shader, vert);
	add_fragment_source(shader, frag);
	for (int i = 0; i < number_of_inputs; ++i)
		add_shader_input(shader, input_name[i], i);
	if (!compile_and_link_shader(shader)) {
		const char *vl = vertex_shader_info_log(shader);
		const char *fl = fragment_shader_info_log(shader);
		const char *sl = shader_info_log(shader);
		if (vl) print_error_message("Vertex SHADER ERROR %s\n---------------\n%s\n-------------------\n", name, vl);
		if (fl) print_error_message("Fragment SHADER ERROR %s\n---------------\n%s\n-------------------\n", name, fl);
		if (sl) print_error_message("Linker SHADER ERROR %s\n---------------\n%s\n-------------------\n", name, sl);
	}

	delete [] input_name;
}


}

