#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>

#include "cgl.h"
#include "mesh.h"
#include "shader.h"
#include "drawelement.h"
#include "camera.h"
#include "texture.h"
#include "framebuffer.h"
#include "glut.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>



shader_ref my_shader;
shader_ref line_shader;
shader_ref tex_shader;
shader_ref pick_shader;

void make_shaders() // {{{
{
	static bool first = true;
	if (!first) return;
	first = false;

	const char *vertex = 
		"// minimal.vert\n"
		"#version 150 core\n"
		"\n"
		"in vec3 in_Position;\n"
		"in vec3 in_normal;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 moview;\n"
		"out vec4 pos_wc;\n"
		"out vec3 normal_wc;\n"
		"in vec3 in_Color;\n"
		"out vec3 ex_Color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	pos_wc = vec4(in_Position, 1.0);\n"
		"	normal_wc = in_normal;\n"
		"	gl_Position = proj * moview * pos_wc;\n"
		"	ex_Color = in_Color;\n"
		"}\n";

	const char *frag = 
		"// minimal.frag\n"
		"#version 150 core\n"
		"\n"
		"// precision highp float;\n"
		"\n"
		"in vec3 ex_Color;\n"
		"in vec4 pos_wc;\n"
		"in vec3 normal_wc;\n"
		"out vec4 out_col;\n"
		"uniform vec3 light_pos;\n"
		"uniform vec2 h_min_max;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	out_col = vec4(normalize(normal_wc), 1);\n"
		"	vec3 to_light = normalize(light_pos - pos_wc.xyz);\n"
		"	float n_dot_l = dot(normal_wc, to_light);\n"
		"	out_col = vec4(n_dot_l,0,0,1);\n"
		"   float r = (pos_wc.y - h_min_max.x) / (h_min_max.y - h_min_max.x);\n"
		"	out_col = vec4(r,0,0,1);\n"
		"	//out_col = vec4(1,0,0,1.0);\n"
		"}\n";
	

	my_shader = make_shader("my shader", 2);
	add_vertex_source(my_shader, vertex);
	add_fragment_source(my_shader, frag);
	add_shader_input(my_shader, "in_Position", 0);
	add_shader_input(my_shader, "in_normal", 1);
	bool ok = compile_and_link_shader(my_shader);
	if (!ok) {
		fprintf(stderr, "Vertex Shader Info Log:\n"
		                "-----------------------\n%s\n"
						"Fragment Shader Info Log:\n"
						"-------------------------\n%s\n"
						"Program Info Log:\n"
						"-----------------\n%s\n", vertex_shader_info_log(my_shader),
						                           fragment_shader_info_log(my_shader),
												   shader_info_log(my_shader));
	}

	const char *line_vert =
		"#version 150 core\n"
		"in vec3 in_pos;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 moview;\n"
		"void main() {\n"
		"	gl_Position = proj * moview * vec4(in_pos,1);\n"
		"}\n";
	const char *line_frag =
		"#version 150 core\n"
		"out vec4 out_col;\n"
		"uniform vec3 line_col;\n"
		"void main() {\n"
		"	out_col = vec4(line_col,1);\n"
		"}\n";
	line_shader = make_shader("line shader", 1);
	add_vertex_source(line_shader, line_vert);
	add_fragment_source(line_shader, line_frag);
	add_shader_input(line_shader, "in_pos", 0);
	ok = compile_and_link_shader(line_shader);
	if (!ok) {
		fprintf(stderr, "Vertex Shader Info Log:\n"
		                "-----------------------\n%s\n"
						"Fragment Shader Info Log:\n"
						"-------------------------\n%s\n"
						"Program Info Log:\n"
						"-----------------\n%s\n", vertex_shader_info_log(line_shader),
						                           fragment_shader_info_log(line_shader),
												   shader_info_log(line_shader));
	}


	const char *tex_vert =
		"#version 150 core\n"
		"in vec3 in_pos;\n"
		"in vec2 in_tc;\n"
		"out vec2 out_tc;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 moview;\n"
		"void main() {\n"
		"	gl_Position = proj * moview * vec4(in_pos,1);\n"
		"	out_tc = in_tc;\n"
		"}\n";
	const char *tex_frag =
		"#version 150 core\n"
		"in vec2 out_tc;\n"
		"out vec4 out_col;\n"
		"uniform sampler2D the_tex;\n"
		"void main() {\n"
		"	out_col = vec4(texture(the_tex, out_tc).rgb, 1);\n"
		"}\n";
	tex_shader = make_shader("line shader", 2);
	add_vertex_source(tex_shader, tex_vert);
	add_fragment_source(tex_shader, tex_frag);
	add_shader_input(tex_shader, "in_pos", 0);
	add_shader_input(tex_shader, "in_tc", 1);
	ok = compile_and_link_shader(tex_shader);
	if (!ok) {
		fprintf(stderr, "Vertex Shader Info Log:\n"
		                "-----------------------\n%s\n"
						"Fragment Shader Info Log:\n"
						"-------------------------\n%s\n"
						"Program Info Log:\n"
						"-----------------\n%s\n", vertex_shader_info_log(tex_shader),
						                           fragment_shader_info_log(tex_shader),
												   shader_info_log(tex_shader));
	}


	const char *pick_vert =
		"#version 150 core\n"
		"in vec3 in_pos;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 moview;\n"
		"void main() {\n"
		"	gl_Position = proj * moview * vec4(in_pos,1);\n"
		"}\n";
	const char *pick_frag =
		"#version 150 core\n"
		"out uvec4 id;\n"
		"uniform uint object_id;\n"
		"void main() {\n"
		"	id = uvec4(object_id, 0, 0, 0);\n"
		"}\n";
	pick_shader = make_shader("line shader", 1);
	add_vertex_source(pick_shader, pick_vert);
	add_fragment_source(pick_shader, pick_frag);
	add_shader_input(pick_shader, "in_pos", 0);
	ok = compile_and_link_shader(pick_shader);
	if (!ok) {
		fprintf(stderr, "Vertex Shader Info Log:\n"
		                "-----------------------\n%s\n"
						"Fragment Shader Info Log:\n"
						"-------------------------\n%s\n"
						"Program Info Log:\n"
						"-----------------\n%s\n", vertex_shader_info_log(pick_shader),
						                           fragment_shader_info_log(pick_shader),
												   shader_info_log(pick_shader));
	}


} // }}}

#define refinement_levels 5
mesh_ref ortho_test, control_point_mesh, control_grid_mesh;
float h_min = 0, h_max = 0;
int level = 0;
int selected_cp = -1;
bool draw_cp = true;

mesh_ref test_mesh;
texture_ref test_texture, fbo_texture;

// 
// glut stuff
// 

#define number_of_control_points 6

struct bezier_node {
	struct bezier_node *left, *right;
	mesh_ref mesh;
	vec3f control_points[number_of_control_points];
	unsigned int indices[number_of_control_points];
};
struct bezier_node *bezier_line;

struct bezier_patch {
	struct bezier_patch *children[4];
	mesh_ref mesh;
	vec3f control_grid[number_of_control_points*number_of_control_points];
	vec2f tex_coords[number_of_control_points*number_of_control_points];
};
struct bezier_patch *bezier_patch;

framebuffer_ref framebuffer;


static void display(void)
{
// 	bind_texture(fbo_texture);
// 	glClearColor(0,1,0,1);
// 	bind_framebuffer(framebuffer);
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	unbind_framebuffer(framebuffer);
// 	unbind_texture(fbo_texture);
	
	glClearColor(0,0,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bind_shader(line_shader);
	
	int loc = glGetUniformLocation(gl_shader_object(line_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
	loc = glGetUniformLocation(gl_shader_object(line_shader), "moview");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	vec3f colors[7];
	make_vec3f(colors+0, 1, 0, 0);
	make_vec3f(colors+1, 0, 1, 0);
	make_vec3f(colors+2, 0, 0, 1);
	make_vec3f(colors+3, 1, 1, 0);
	make_vec3f(colors+4, 0, 1, 1);
	make_vec3f(colors+5, 1, 0, 1);
	make_vec3f(colors+6, 1, 1, 1);


	
	bind_mesh_to_gl(ortho_test);
	glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(ortho_test), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(ortho_test);

	loc = glGetUniformLocation(gl_shader_object(line_shader), "line_col");
		
	if (draw_cp) {
		bind_mesh_to_gl(control_grid_mesh);
		glUniform3fv(loc, 1, (float*)(colors+6));
		glDrawElements(GL_LINES, index_buffer_length_of_mesh(control_grid_mesh), GL_UNSIGNED_INT, 0);
		unbind_mesh_from_gl(control_point_mesh);
	}
	
	glPointSize(10);
	bind_mesh_to_gl(control_point_mesh);
	glUniform3fv(loc, 1, (float*)(colors+3));
	if (selected_cp >= 0)
// 		glDrawRangeElements(GL_POINTS, selected_cp+1, selected_cp+2, 2, GL_UNSIGNED_INT, 0);
		glDrawElementsBaseVertex(GL_POINTS, 1, GL_UNSIGNED_INT, 0, selected_cp);
	glUniform3fv(loc, 1, (float*)(colors+6));
	glDrawElements(GL_POINTS, index_buffer_length_of_mesh(control_point_mesh), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(control_point_mesh);
	
	unbind_shader(line_shader);
	bind_shader(tex_shader);

	void render_patch(struct bezier_patch *patch, int level) {
		if (level == 0) {
			bind_mesh_to_gl(patch->mesh);
			glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(patch->mesh), GL_UNSIGNED_INT, 0);
			unbind_mesh_from_gl(patch->mesh);
		}
		else for (int i = 0; i < 4; ++i)
			render_patch(patch->children[i], level-1);
	}
	loc = glGetUniformLocation(gl_shader_object(tex_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
	loc = glGetUniformLocation(gl_shader_object(tex_shader), "moview");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	
	glBindTexture(GL_TEXTURE_2D, texture_id(test_texture));
	loc = glGetUniformLocation(gl_shader_object(tex_shader), "the_tex");
	glUniform1i(loc, 0);

	render_patch(bezier_patch, level);

	unbind_shader(tex_shader);
	


	bind_shader(tex_shader);

	/*
	loc = glGetUniformLocation(gl_shader_object(tex_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
// 	loc = glGetUniformLocation(gl_shader_object(tex_shader), "moview");
// 	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

	glBindTexture(GL_TEXTURE_2D, texture_id(test_texture));
	loc = glGetUniformLocation(gl_shader_object(tex_shader), "the_tex");
	glUniform1i(loc, 0);

	bind_mesh_to_gl(test_mesh);
	glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(test_mesh), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(test_mesh);
	unbind_shader(tex_shader);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/

// 	unbind_framebuffer(framebuffer);

	swap_buffers();
	check_for_gl_errors("display");
	
// 	save_texture_as_rgb_png(fbo_texture, "/tmp/out.png");
}


struct bezier_node* make_bezier_segment(vec3f *cp, int d);

void subdivide(vec3f *control_points, vec2f *tc, vec3f *left, vec3f *right, vec2f *texleft, vec2f *texright) {
	vec3f level[number_of_control_points][number_of_control_points];
	vec3f tmp;
	vec2f leveltc[number_of_control_points][number_of_control_points];
	vec2f tmp2;

	for (int i = 0; i < number_of_control_points; ++i) {
		level[0][i] = control_points[i];
		leveltc[0][i] = tc[i];
	}
	left[0] = control_points[0];
	right[number_of_control_points-1] = control_points[number_of_control_points-1];
	texleft[0] = tc[0];
	texright[number_of_control_points-1] = tc[number_of_control_points-1];


	for (int l = 1; l < number_of_control_points; ++l) {
		for (int i = 0; i < number_of_control_points - l; ++i) {
			sub_components_vec3f(&tmp, level[l-1]+i+1, level[l-1]+i);
			div_vec3f_by_scalar(&tmp, &tmp, 2);
			add_components_vec3f(level[l]+i, level[l-1]+i, &tmp);

			sub_components_vec2f(&tmp2, leveltc[l-1]+i+1, leveltc[l-1]+i);
			div_vec2f_by_scalar(&tmp2, &tmp2, 2);
			add_components_vec2f(leveltc[l]+i, leveltc[l-1]+i, &tmp2);
		}
		left[l] = level[l][0];
		right[number_of_control_points-l-1] = level[l][number_of_control_points-l-1];
		texleft[l] = leveltc[l][0];
		texright[number_of_control_points-l-1] = leveltc[l][number_of_control_points-l-1];
	}
}

#define number_of_patch_indices (2*(2*(number_of_control_points-1)*number_of_control_points))
unsigned int patch_indices[number_of_patch_indices];

#define number_of_patch_tri_indices (6*(number_of_control_points-1)*(number_of_control_points-1))
unsigned int patch_tri_indices[number_of_patch_tri_indices];

void precompute_index_buffer() {
	int w = number_of_control_points, h = number_of_control_points;
	for (int y = 0; y < h-1; ++y)
		for (int x = 0; x < w-1; ++x) {
			int idx = (y*(w-1)+x);
			patch_indices[4*idx + 0] = y*w+x;
			patch_indices[4*idx + 1] = y*w+x+1;
			patch_indices[4*idx + 2] = y*w+x;
			patch_indices[4*idx + 3] = (y+1)*w+x;
		}
	int sofar = (h-1)*(w-1)*4;
	for (int y = 0; y < h-1; ++y) {
		patch_indices[sofar + 2*y + 0] = y*w + w-1;
		patch_indices[sofar + 2*y + 1] = (y+1)*w + w-1;
	}
	sofar += 2*(h-1);
	for (int x = 0; x < w-1; ++x) {
		patch_indices[sofar + 2*x + 0] = (h-1)*w + x;
		patch_indices[sofar + 2*x + 1] = (h-1)*w + x+1;
	}
	printf("sofar: %d / %d\n", sofar + 2*(w-1), 2*(2*(number_of_control_points-1)*number_of_control_points));

	for (int y = 0; y < h-1; ++y)
		for (int x = 0; x < w-1; ++x) {
			int base = 6*(y*(w-1)+x);
			patch_tri_indices[base+0] = ((y+0)*w) + x+0;
			patch_tri_indices[base+1] = ((y+0)*w) + x+1;
			patch_tri_indices[base+2] = ((y+1)*w) + x+1;
			patch_tri_indices[base+3] = ((y+0)*w) + x+0;
			patch_tri_indices[base+4] = ((y+1)*w) + x+1;
			patch_tri_indices[base+5] = ((y+1)*w) + x+0;
		}
}

void subdivide_patch(struct bezier_patch *p, int d);

struct bezier_patch* make_bezier_patch(vec3f *cp, vec2f *tc, int d) {
	// init patch
	struct bezier_patch *patch = malloc(sizeof(struct bezier_patch));
	for (int i = 0; i < 4; ++i) patch->children[i] = 0;
	// copy control points
	for (int y = 0; y < number_of_control_points; ++y)
		for (int x = 0; x < number_of_control_points; ++x) {
			patch->control_grid[y*number_of_control_points+x] = cp[y*number_of_control_points+x];
			patch->tex_coords[y*number_of_control_points+x] = tc[y*number_of_control_points+x];
		}
	patch->mesh = make_mesh("a bezier patch", 2);
	bind_mesh_to_gl(patch->mesh);
	add_vertex_buffer_to_mesh(patch->mesh, "vt", GL_FLOAT, number_of_control_points*number_of_control_points, 3, patch->control_grid, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(patch->mesh, "tc", GL_FLOAT, number_of_control_points*number_of_control_points, 2, patch->tex_coords, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(patch->mesh, number_of_patch_tri_indices, patch_tri_indices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(patch->mesh);

	if (d > 0) subdivide_patch(patch, d);

	return patch;
}

void subdivide_patch(struct bezier_patch *p, int d) {
	vec3f left[number_of_control_points],
	      right[number_of_control_points],
	      curr[number_of_control_points];
	vec3f new_grids[4][number_of_control_points*number_of_control_points];
	vec2f texleft[number_of_control_points],
	      texright[number_of_control_points],
	      texcurr[number_of_control_points];
	vec2f new_texs[4][number_of_control_points*number_of_control_points];
	int w = number_of_control_points/*, h = number_of_control_points*/;
	
	// subdivide along y
	for (int y = 0; y < number_of_control_points; ++y) {
		for (int x = 0; x < number_of_control_points; ++x) {
			curr[x] = p->control_grid[y*w + x];
			texcurr[x] = p->tex_coords[y*w +x];
		}
		subdivide(curr, texcurr, left, right, texleft, texright);
		for (int x = 0; x < number_of_control_points; ++x) {
			new_grids[0][y*w+x] = left[x];
			new_grids[1][y*w+x] = right[x];
			new_texs[0][y*w+x] = texleft[x];
			new_texs[1][y*w+x] = texright[x];
		}
	}
	
	// subdivide along x
	for (int x = 0; x < number_of_control_points; ++x) {
		for (int i = 0; i < 2; ++i) {
			for (int y = 0; y < number_of_control_points; ++y) {
				curr[y] = new_grids[i][y*w + x];
				texcurr[y] = new_texs[i][y*w +x];
			}
			subdivide(curr, texcurr, left, right, texleft, texright);
			for (int y = 0; y < number_of_control_points; ++y) {
				new_grids[i][y*w+x] = left[y];
				new_grids[i+2][y*w+x] = right[y];
				new_texs[i][y*w+x] = texleft[y];
				new_texs[i+2][y*w+x] = texright[y];
			}
		}
	}

	for (int i = 0; i < 4; ++i)
		p->children[i] = make_bezier_patch(new_grids[i], new_texs[i], d-1);
}

vec3f base_control_grid[number_of_control_points*number_of_control_points];
vec2f base_tex_grid[number_of_control_points*number_of_control_points];
unsigned int cp_ids[number_of_control_points*number_of_control_points];

void regen_patch() {
	static bool first_time = true;
	float z = 10;
	
	if (first_time)
		for (int y = 0; y < number_of_control_points; ++y)
			for (int x = 0; x < number_of_control_points; ++x) {
				make_vec3f(base_control_grid+y*number_of_control_points+x, x/2.0-0.5, y/2.0-0.5, z);
				make_vec2f(base_tex_grid+y*number_of_control_points+x, x/(float)(number_of_control_points-1), y/(float)(number_of_control_points-1));
				cp_ids[y*number_of_control_points+x] = y*number_of_control_points+x;
			}
	
	bezier_patch = make_bezier_patch(base_control_grid, base_tex_grid, 5);
	
	if (first_time) {
		control_point_mesh = make_mesh("control points", 1);
		bind_mesh_to_gl(control_point_mesh);
		add_vertex_buffer_to_mesh(control_point_mesh, "vt", GL_FLOAT, number_of_control_points*number_of_control_points, 3, base_control_grid, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(control_point_mesh, number_of_control_points*number_of_control_points, cp_ids, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_point_mesh);

		control_grid_mesh = make_mesh("control grid", 1);
		bind_mesh_to_gl(control_grid_mesh);
		add_vertex_buffer_to_mesh(control_grid_mesh, "vt", GL_FLOAT, number_of_control_points*number_of_control_points, 3, base_control_grid, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(control_grid_mesh, number_of_patch_indices, patch_indices, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_grid_mesh);

		first_time = false;
	}
	else {
		bind_mesh_to_gl(control_point_mesh);
		change_vertex_buffer_data(control_point_mesh, "vt", GL_FLOAT, 3, base_control_grid, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_point_mesh);
		
		bind_mesh_to_gl(control_grid_mesh);
		change_vertex_buffer_data(control_grid_mesh, "vt", GL_FLOAT, 3, base_control_grid, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_grid_mesh);
	}

}

void spline_keyhandler(unsigned char key, int x, int y) {
	if (key == '+' && level < 3) ++level;
	else if (key == '-' && level > 0) --level;
	else if (key == 'v') {
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		printf("VP: %d %d %d %d\n", vp[0], vp[1], vp[2], vp[3]);
	}
	else if (key == ' ')
		draw_cp = !draw_cp;
	
printf("key\n");

	if (selected_cp != -1) {
	printf("cp!\n");
		bool regen = true;
		switch (key) {
			case 'x': base_control_grid[selected_cp].x -= 0.5; break;
			case 'X': base_control_grid[selected_cp].x += 0.5; break;
			case 'y': base_control_grid[selected_cp].y -= 0.5; break;
			case 'Y': base_control_grid[selected_cp].y += 0.5; break;
			case 'z': base_control_grid[selected_cp].z -= 0.5; break;
			case 'Z': base_control_grid[selected_cp].z += 0.5; break;
			default: regen = false;
		}
		if (regen)
			regen_patch();
	}

// 	regen_patch();
// 	regen_bezier();
	standard_keyboard(key, x, y);
}

vec4f map_glut_coordinates_to_ortho(int x, int y) {
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float xx = ((float)x-vp[0])/vp[2];
	float yy = ((float)y-vp[1])/vp[3];
	xx = xx*2.0 - 1.0;
	yy = -(yy*2.0 - 1.0);

	matrix4x4f *proj = projection_matrix_of_cam(current_camera());
	matrix4x4f rev;
	invert_matrix4x4f(&rev, proj);

	vec4f v; make_vec4f(&v, xx, yy, 10, 1);
	vec4f mapped;
	multiply_matrix4x4f_vec4f(&mapped, &rev, &v);

	return mapped;
}

bool left_down = false;
static void mouse_motion(int x, int y) {
	
	standard_mouse_motion(x, y);
	
}

void pick(int x, int y);

static void mouse_button(int button, int state, int x, int y) {
	standard_mouse_func(GLUT_LEFT_BUTTON, state, x, y);

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		/*
		float dist = 1e10;
		float max_dist = 0.2;
		vec4f mapped = map_glut_coordinates_to_ortho(x, y);

		selected_cp = -1;
		for (int i = 0; i < number_of_control_points*number_of_control_points; ++i) {
			float dx = mapped.x - base_control_grid[i].x;
			float dy = mapped.y - base_control_grid[i].y;
			float d = dx*dx + dy*dy;
			if (d < dist) {
				dist = d;
				if (dist <= max_dist)
					selected_cp = i;
			}
		}

		left_down = true;
		*/
		pick(x, 500-y);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_down = false;
// 		regen_patch();
	}
}

framebuffer_ref picking_fbo;
texture_ref picking_tex;

void initialize_picking(int w, int h) {
	picking_tex = make_empty_texture("pick-ids", w, h, GL_TEXTURE_2D, GL_R32UI, GL_UNSIGNED_INT, GL_RGB_INTEGER);
	picking_fbo = make_framebuffer("picking-fbo", w, h);
	bind_framebuffer(picking_fbo);
	bind_texture(picking_tex);
	attach_texture_as_colorbuffer(picking_fbo, "content", picking_tex);
	attach_depth_buffer(picking_fbo);
	check_framebuffer_setup(picking_fbo);
	unbind_framebuffer(picking_fbo);
	unbind_texture(picking_tex);
}

void pick(int x, int y) {
	bind_framebuffer(picking_fbo);
	
	bind_shader(pick_shader);
	bind_mesh_to_gl(control_point_mesh);
		int loc = glGetUniformLocation(gl_shader_object(pick_shader), "proj");
		glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
		loc = glGetUniformLocation(gl_shader_object(pick_shader), "moview");
		glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
		
		glPointSize(10);
		loc = glGetUniformLocation(gl_shader_object(pick_shader), "object_id");

		for (int i = 0; i < number_of_control_points*number_of_control_points; ++i) {
			glUniform1ui(loc, i+1);
			glDrawElementsBaseVertex(GL_POINTS, 1, GL_UNSIGNED_INT, 0, i);
		}
	unbind_mesh_from_gl(control_point_mesh);
	unbind_shader(pick_shader);

	unbind_framebuffer(picking_fbo);

	bind_framebuffer(framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	unsigned int data[4];
	glReadPixels(x, y, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, data);
	unbind_framebuffer(framebuffer);

	selected_cp = data[0]-1;
}

int main(int argc, char **argv) 
{
	startup_cgl("blub", 3, 3, argc, argv, 500, 500, true);
	register_display_function(display);
	register_idle_function(display);
	register_keyboard_function(spline_keyhandler);
	register_mouse_function(mouse_button);
	register_mouse_motion_function(mouse_motion);

	make_shaders();

	vec3f cam_pos, cam_dir, cam_up;
	make_vec3f(&cam_pos, 0, 0, 5);
	make_vec3f(&cam_dir, 0, 0, 1);
	make_vec3f(&cam_up, 0, 1, 0);
	use_camera(make_perspective_cam("my cam", &cam_pos, &cam_dir, &cam_up, 45, 1, 0.1, 1000));
// 	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, (int)2*M_PI*100, 0, 100, 0, 0.1, 1000));
	float ortho_r = 2, ortho_l = -1,
	      ortho_t = 2, ortho_b = -1;
// 	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, ortho_r, ortho_l, ortho_t, ortho_b, 0.01, 1000));

	// -------------
	
	vec3f *ortho_verts = malloc(sizeof(vec3f)*3*4);
	unsigned int *ortho_ids = malloc(sizeof(unsigned int) * 12);
	
	float z = 10;
	float x_fraction = (ortho_r - ortho_l) / 10.0f;
	float y_fraction = (ortho_t - ortho_b) / 10.0f;
	make_vec3f(ortho_verts+0, ortho_r, ortho_t, z);
	make_vec3f(ortho_verts+1, ortho_r, ortho_t-y_fraction, z);
	make_vec3f(ortho_verts+2, ortho_r-x_fraction, ortho_t, z);
	
	make_vec3f(ortho_verts+3, ortho_r, ortho_b, z);
	make_vec3f(ortho_verts+4, ortho_r, ortho_b+y_fraction, z);
	make_vec3f(ortho_verts+5, ortho_r-x_fraction, ortho_b, z);
	
	make_vec3f(ortho_verts+6, ortho_l, ortho_b, z);
	make_vec3f(ortho_verts+7, ortho_l, ortho_b+y_fraction, z);
	make_vec3f(ortho_verts+8, ortho_l+x_fraction, ortho_b, z);

	make_vec3f(ortho_verts+9,  ortho_l, ortho_t, z);
	make_vec3f(ortho_verts+10, ortho_l, ortho_t-y_fraction, z);
	make_vec3f(ortho_verts+11, ortho_l+x_fraction, ortho_t, z);

	for (int i = 0; i < 12; ++i)
		ortho_ids[i] = i;

	ortho_test = make_mesh("test", 1);
	bind_mesh_to_gl(ortho_test);
	add_vertex_buffer_to_mesh(ortho_test, "ot", GL_FLOAT, 12, 3, (float*)ortho_verts, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(ortho_test, 12, ortho_ids, GL_STATIC_DRAW);
	unbind_mesh_from_gl(ortho_test);

	vec3f testmesh_v[3] = { {0,0,10}, {0,1,10}, {1,1,10} };
	vec2f testmesh_t[3] = { {0,0}, {0,1}, {1,1} };
	unsigned int testmesh_i[3] = {0,1,2};
	test_mesh = make_mesh("test", 2);
	bind_mesh_to_gl(test_mesh);
	add_vertex_buffer_to_mesh(test_mesh, "vt", GL_FLOAT, 3, 3, testmesh_v, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(test_mesh, "tx", GL_FLOAT, 3, 2, testmesh_t, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(test_mesh, 3, testmesh_i, GL_STATIC_DRAW);
	unbind_mesh_from_gl(test_mesh);

	test_texture = make_texture("mytex", "/home/kai/Downloads/a-gnu.png", GL_TEXTURE_2D);
	/*
	fbo_texture = make_empty_texture("fbo content", 256, 256, GL_TEXTURE_2D);
	framebuffer = make_framebuffer("the-fbo", 256, 256);
	bind_framebuffer(framebuffer);
	glBindTexture(GL_TEXTURE_2D, texture_id(fbo_texture));
	attach_texture_as_colorbuffer(framebuffer, "content", fbo_texture);
	attach_depth_buffer(framebuffer);
	check_framebuffer_setup(framebuffer);
	unbind_framebuffer(framebuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/

	initialize_picking(500, 500);
	// -------------
// 	regen_bezier();
	precompute_index_buffer();
	regen_patch();
	// -------------

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	enter_glut_main_loop();

	return 0;
}

// vim: set foldmethod=marker :
