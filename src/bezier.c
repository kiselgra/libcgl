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
#include "glut.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>



shader_ref my_shader;
shader_ref line_shader;

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
		"void main() {\n"
		"	gl_Position = proj * vec4(in_pos,1);\n"
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


} // }}}

#define refinement_levels 5
mesh_ref s1, line_mesh[7][7], ortho_test, control_point_mesh, spline_mesh, refined_cps[refinement_levels];
float h_min = 0, h_max = 0;
int level = 0;
int selected_cp = -1;
bool draw_cp = true;


// 
// glut stuff
// 

#define number_of_control_points 8

struct bezier_node {
	struct bezier_node *left, *right;
	mesh_ref mesh;
	vec3f control_points[number_of_control_points];
	unsigned int indices[number_of_control_points];
};
struct bezier_node *bezier_line;

static void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bind_shader(line_shader);
	
	int loc = glGetUniformLocation(gl_shader_object(line_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
	vec3f colors[7];
	make_vec3f(colors+0, 1, 0, 0);
	make_vec3f(colors+1, 0, 1, 0);
	make_vec3f(colors+2, 0, 0, 1);
	make_vec3f(colors+3, 1, 1, 0);
	make_vec3f(colors+4, 0, 1, 1);
	make_vec3f(colors+5, 1, 0, 1);
	make_vec3f(colors+6, 1, 1, 1);

/*
	for (int i = 0; i < 7-level; ++i) {
		int loc = glGetUniformLocation(gl_shader_object(line_shader), "line_col");
		glUniform3fv(loc, 1, (float*)(colors+i));
		bind_mesh_to_gl(line_mesh[level][i]);
		glDrawElements(GL_LINE_STRIP, index_buffer_length_of_mesh(line_mesh[level][i]), GL_UNSIGNED_INT, 0);
		unbind_mesh_from_gl(line_mesh[level][i]);
	}
	*/

	
	bind_mesh_to_gl(ortho_test);
	glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(ortho_test), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(ortho_test);

	loc = glGetUniformLocation(gl_shader_object(line_shader), "line_col");
	bind_mesh_to_gl(control_point_mesh);
	glUniform3fv(loc, 1, (float*)(colors+3));
	if (selected_cp >= 0)
// 		glDrawRangeElements(GL_POINTS, selected_cp+1, selected_cp+2, 2, GL_UNSIGNED_INT, 0);
		glDrawElementsBaseVertex(GL_POINTS, 1, GL_UNSIGNED_INT, 0, selected_cp);
	glUniform3fv(loc, 1, (float*)(colors+6));
	if (draw_cp)
		glDrawElements(GL_LINE_STRIP, index_buffer_length_of_mesh(control_point_mesh), GL_UNSIGNED_INT, 0);
	glPointSize(10);
	glDrawElements(GL_POINTS, index_buffer_length_of_mesh(control_point_mesh), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(control_point_mesh);
	
	void render_segment(struct bezier_node *node, int level) {
		if (level == 0) {
			bind_mesh_to_gl(node->mesh);
			glDrawElements(GL_LINE_STRIP, index_buffer_length_of_mesh(node->mesh), GL_UNSIGNED_INT, 0);
			unbind_mesh_from_gl(node->mesh);
		}
		else if (node->left && node->right) {
			render_segment(node->left, level-1);
			render_segment(node->right, level-1);
		}
	}
	
	glUniform3fv(loc, 1, (float*)(colors+0));
	render_segment(bezier_line, level);
	
	/*
	glUniform3fv(loc, 1, (float*)(colors+0));
	bind_mesh_to_gl(spline_mesh);
	glDrawElements(GL_LINE_STRIP, index_buffer_length_of_mesh(spline_mesh), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(spline_mesh);
	*/


	
	unbind_shader(line_shader);

	swap_buffers();
	check_for_gl_errors("display");
}


static vec3f control_points[number_of_control_points];
unsigned int cp_ids[number_of_control_points];

struct bezier_node* make_bezier_segment(vec3f *cp, int d);

void subdivide(struct bezier_node *node, int d) {
/*
	vec3f left[4], right[4];
	vec3f level1[3];
	vec3f level2[2];
	vec3f level3[1];
	vec3f tmp;
	sub_components_vec3f(&tmp, node->control_points+1, node->control_points+0);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level1+0, node->control_points+0, &tmp);
	sub_components_vec3f(&tmp, node->control_points+2, node->control_points+1);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level1+1, node->control_points+1, &tmp);
	sub_components_vec3f(&tmp, node->control_points+3, node->control_points+2);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level1+2, node->control_points+2, &tmp);
	
	sub_components_vec3f(&tmp, level1+1, level1+0);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level2+0, level1+0, &tmp);
	sub_components_vec3f(&tmp, level1+2, level1+1);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level2+1, level1+1, &tmp);
	
	sub_components_vec3f(&tmp, level2+1, level2+0);    div_vec3f_by_scalar(&tmp, &tmp, 2);   add_components_vec3f(level3+0, level2+0, &tmp);

	left[0] = node->control_points[0];
	left[1] = level1[0];
	left[2] = level2[0];
	left[3] = level3[0];
	
	right[0] = level3[0];
	right[1] = level2[1];
	right[2] = level1[2];
	right[3] = node->control_points[3];
*/
	vec3f left[number_of_control_points], right[number_of_control_points];
	vec3f level[number_of_control_points][number_of_control_points];
	vec3f tmp;

	for (int i = 0; i < number_of_control_points; ++i)
		level[0][i] = node->control_points[i];
	left[0] = node->control_points[0];
	right[number_of_control_points-1] = node->control_points[number_of_control_points-1];

	printf("------------------\n");
	for (int l = 1; l < number_of_control_points; ++l) {
// 		printf("level %d: ", l);
		for (int i = 0; i < number_of_control_points - l; ++i) {
// 			printf("%d ", i);
			sub_components_vec3f(&tmp, level[l-1]+i+1, level[l-1]+i);
			div_vec3f_by_scalar(&tmp, &tmp, 2);
			add_components_vec3f(level[l]+i, level[l-1]+i, &tmp);
		}
// 		printf("\n");
// 		printf("left %d:  L %d   I %d\n", l, l, 0);
// 		printf("right %d: L %d   I %d\n", number_of_control_points-l-1, l, number_of_control_points-l-1);
		left[l] = level[l][0];
		right[number_of_control_points-l-1] = level[l][number_of_control_points-l-1];
	}
	
	node->left = make_bezier_segment(left, d+1);
	node->right = make_bezier_segment(right, d+1);
}

struct bezier_node* make_bezier_segment(vec3f *cp, int d) {
	struct bezier_node *node = malloc(sizeof(struct bezier_node));
	node->left = node->right = 0;
	for (int i = 0; i < number_of_control_points; ++i) {
		node->control_points[i] = cp[i];
		node->indices[i] = i;
	}
	node->mesh = make_mesh("a bezier segment", 1);
	bind_mesh_to_gl(node->mesh);
	add_vertex_buffer_to_mesh(node->mesh, "vt", GL_FLOAT, number_of_control_points, 3, node->control_points, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(node->mesh, number_of_control_points, node->indices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(node->mesh);

	if (d < 5) subdivide(node, d);
	return node;
}

void regen_bezier() {
	static bool first_time = true;
	float z = 10;

	for (int i = 0; i < number_of_control_points; ++i) cp_ids[i] = i;

	if (first_time) {
		make_vec3f(control_points+0, 0,    -0.5, z);
		make_vec3f(control_points+1, -0.5,  0.5, z);
		make_vec3f(control_points+2,  0.5,  1.5, z);
		make_vec3f(control_points+3,  1.5,  1.5, z);
		for (int i = 4; i < number_of_control_points; ++i)
			make_vec3f(control_points+i, 0.5+i*0.2, 0.0, z);
	}
	
	bezier_line = make_bezier_segment(control_points, 0);

	if (first_time) {
		control_point_mesh = make_mesh("control points", 1);
		bind_mesh_to_gl(control_point_mesh);
		add_vertex_buffer_to_mesh(control_point_mesh, "vt", GL_FLOAT, number_of_control_points, 3, control_points, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(control_point_mesh, number_of_control_points, cp_ids, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_point_mesh);

		first_time = false;
	}
	else {
		bind_mesh_to_gl(control_point_mesh);
		change_vertex_buffer_data(control_point_mesh, "vt", GL_FLOAT, 3, control_points, GL_STATIC_DRAW);
		unbind_mesh_from_gl(control_point_mesh);
	}
}

void spline_keyhandler(unsigned char key, int x, int y) {
	if (key == '+' && level < 3) ++level;
	else if (key == '-' && level > 0) --level;
	else if (key == 'x') {
		make_vec3f(control_points+2, 0.5, vec3f_component_val(control_points+2, 1)-0.2, 10);
		regen_bezier();
	}
	else if (key == 'v') {
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		printf("VP: %d %d %d %d\n", vp[0], vp[1], vp[2], vp[3]);
	}
	else if (key == ' ')
		draw_cp = !draw_cp;
	else standard_keyboard(key, x, y);
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
	if (!left_down) return;
	if (selected_cp < 0) return;
	
	printf("%d %d\n", x, y);
	vec4f mapped = map_glut_coordinates_to_ortho(x, y);
	control_points[selected_cp].x = mapped.x;
	control_points[selected_cp].y = mapped.y;
	regen_bezier();
}

static void mouse_button(int button, int state, int x, int y) {
// 	standard_mouse_func(button, state, x, y);
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		float dist = 1e10;
		float max_dist = 0.2;
		vec4f mapped = map_glut_coordinates_to_ortho(x, y);

		selected_cp = -1;
		for (int i = 0; i < number_of_control_points; ++i) {
			float dx = mapped.x - control_points[i].x;
			float dy = mapped.y - control_points[i].y;
			float d = dx*dx + dy*dy;
			if (d < dist) {
				dist = d;
				if (dist <= max_dist)
					selected_cp = i;
			}
		}

		left_down = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_down = false;
		selected_cp = -1;
	}
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
	make_vec3f(&cam_pos, 0, 0, 0);
	make_vec3f(&cam_dir, 0, 0, -1);
	make_vec3f(&cam_up, 0, 1, 0);
	use_camera(make_perspective_cam("my cam", &cam_pos, &cam_dir, &cam_up, 45, 1, 0.1, 1000));
// 	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, (int)2*M_PI*100, 0, 100, 0, 0.1, 1000));
	float ortho_r = 2, ortho_l = -1,
	      ortho_t = 2, ortho_b = -1;
	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, ortho_r, ortho_l, ortho_t, ortho_b, 0.01, 1000));

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

	// -------------
	regen_bezier();
	// -------------

/*
	int w = 10, h = 10;
	vec3f *grid = malloc(sizeof(vec3f)*h*w);
	vec3f *norm = malloc(sizeof(vec3f)*h*w);
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			float z = (rand()%16000-8000)/6000.0;
			make_vec3f(grid+(y*w)+x, x, z, -y);
		}
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			vec3f accum;
			vec3f from;
			copy_vec3f(&from, grid+(y*w)+x);
			vec3f tmp;
			sub_components_vec3f(&accum, grid+((y+1)*w)+x, &from);
			sub_components_vec3f(&tmp, grid+((y-1)*w)+x, &from); add_components_vec3f(&accum, &accum, &tmp);
			sub_components_vec3f(&tmp, grid+(y*w)+x+1, &from);   add_components_vec3f(&accum, &accum, &tmp);
			sub_components_vec3f(&tmp, grid+(y*w)+x-1, &from);   add_components_vec3f(&accum, &accum, &tmp);
			div_vec3f_by_scalar(norm+(y*w)+x, &tmp, 4);
			if (from.y > h_max) h_max = from.y;
			if (from.y < h_min) h_min = from.y;
		}
	int indices = (h-1)*(w-1)*6;
	unsigned int *index_buffer = malloc(sizeof(int)*indices);
	for (int y = 0; y < h-1; ++y)
		for (int x = 0; x < w-1; ++x) {
			index_buffer[6*(y*(w-1)+x) + 0] = y*w+x;
			index_buffer[6*(y*(w-1)+x) + 1] = y*w+x+1;
			index_buffer[6*(y*(w-1)+x) + 2] = (y+1)*w+x;
			index_buffer[6*(y*(w-1)+x) + 3] = y*w+x+1;
			index_buffer[6*(y*(w-1)+x) + 4] = (y+1)*w+x+1;
			index_buffer[6*(y*(w-1)+x) + 5] = (y+1)*w+x;
		}
	s1 = make_mesh("s1", 2);
	bind_mesh_to_gl(s1);
	add_vertex_buffer_to_mesh(s1, "verts", GL_FLOAT, w*h, 3, (float*)grid, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(s1, "normals", GL_FLOAT, w*h, 3, (float*)norm, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(s1, indices, index_buffer, GL_STATIC_DRAW);
	unbind_mesh_from_gl(s1);
	*/

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	enter_glut_main_loop();

	return 0;
}

// vim: set foldmethod=marker :
