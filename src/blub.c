#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glxew.h>

#include "cgl.h"
#include "mesh.h"
#include "shader.h"
#include "drawelement.h"
#include "camera.h"
#include "glut.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>


GLuint master_vertex_array_buffer_object;
GLuint tri_verts, tri_indices;
mesh_ref tri_mesh;
float verts[3*4];
unsigned int indices[3];

const int vertex_attrib_index = 0;

void MakeTriangleNew()
{
	verts[0*4 + 0] = -0.5;
	verts[0*4 + 1] = 0;
	verts[0*4 + 2] = -1;
	verts[0*4 + 3] = 1;
	
	verts[1*4 + 0] = 0;
	verts[1*4 + 1] = 0.5;
	verts[1*4 + 2] = -1;
	verts[1*4 + 3] = 1;
	
	verts[2*4 + 0] = 0.5;
	verts[2*4 + 1] = 0;
	verts[2*4 + 2] = -1;
	verts[2*4 + 3] = 1;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	tri_mesh = make_mesh("blub", 1);

	bind_mesh_to_gl(tri_mesh);
	add_vertex_buffer_to_mesh(tri_mesh, "vertex data", GL_FLOAT, sizeof(float) * 3 * 4, 4, verts, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(tri_mesh, 3, indices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(tri_mesh);
}




shader_ref my_shader;
shader_ref line_shader;

void make_shaders()
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


}

mesh_ref s1, line_mesh[7][7], ortho_test;
float h_min = 0, h_max = 0;
int level = 3;

void render_tri_new()
{
/*
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	bind_shader(my_shader);
	int loc = glGetUniformLocation(gl_shader_object(my_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
	loc = glGetUniformLocation(gl_shader_object(my_shader), "moview");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	vec3f light; make_vec3f(&light, 0, 5, 0);
	loc = glGetUniformLocation(gl_shader_object(my_shader), "light_pos");
	glUniform3fv(loc, 1, (float*)&light);
	vec2f heights; make_vec2f(&heights, h_min, h_max);
	loc = glGetUniformLocation(gl_shader_object(my_shader), "h_min_max");
	glUniform2fv(loc, 1, (float*)&heights);

	bind_mesh_to_gl(s1);
	glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(s1), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(s1);
	
	unbind_shader(my_shader);
	*/

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

	for (int i = 0; i < 7-level; ++i) {
		int loc = glGetUniformLocation(gl_shader_object(line_shader), "line_col");
		glUniform3fv(loc, 1, (float*)(colors+i));
		bind_mesh_to_gl(line_mesh[level][i]);
		glDrawElements(GL_LINE_STRIP, index_buffer_length_of_mesh(line_mesh[level][i]), GL_UNSIGNED_INT, 0);
		unbind_mesh_from_gl(line_mesh[level][i]);
	}

	
	bind_mesh_to_gl(ortho_test);
	glDrawElements(GL_TRIANGLES, index_buffer_length_of_mesh(ortho_test), GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(ortho_test);
	
	unbind_shader(line_shader);
}

// 
// glut stuff
// 

static void display(void)
{
	static float c = 0;
	//c += 0.00001;
	if (c > 2.0f) c = 0;
	//glClearColor(c>1.0f ? 2.0-c : c,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_tri_new();

	swap_buffers();
// 	glutPostRedisplay();
	check_for_gl_errors("display");
}

mesh_ref make_sampled_function_mesh(int samples, float (*func)(float), float start, float end) {
	vec3f *line_verts = malloc(sizeof(vec3f) * samples);
	unsigned int *line_indices = malloc(sizeof(unsigned int)*samples);
	for (int i = 0; i < samples; ++i) {
		float x = ((end-start) / (float)(samples-1)) * i + start;
		float y = func(x);
		make_vec3f(line_verts + i, x, y, 10);
		line_indices[i] = i;
	}
	
	mesh_ref sample_mesh = make_mesh("sampled function", 1);
	bind_mesh_to_gl(sample_mesh);
	add_vertex_buffer_to_mesh(sample_mesh, "verts", GL_FLOAT, samples, 3, (float*)line_verts, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(sample_mesh, samples, line_indices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(sample_mesh);
	return sample_mesh;
}

float N(int k, int i, float u, float *t) {
	if (k == 0) {
		if (t[i] <= u && u < t[i+1]) return 1.0f;
		else                         return 0.0f;
	}
	float a = (u - t[i]) / (t[i+k] - t[i]);
	float b = (t[i+k+1] - u) / (t[i+k+1] - t[i+1]);
	float Na = N(k-1, i, u, t);
	float Nb = N(k-1, i+1, u, t);
	return a * Na + b * Nb;
}

mesh_ref make_sampled_bspline_mesh(int samples, int k, int i, float *t, float start, float end) {
	vec3f *line_verts = malloc(sizeof(vec3f) * samples);
	unsigned int *line_indices = malloc(sizeof(unsigned int)*samples);
	for (int s = 0; s < samples; ++s) {
		float x = ((end-start) / (float)(samples-1)) * s + start;
		float y = N(k, i, x, t);
		make_vec3f(line_verts + s, x, y, 10);
		line_indices[s] = s;
	}
	
	mesh_ref sample_mesh = make_mesh("sampled function", 1);
	bind_mesh_to_gl(sample_mesh);
	add_vertex_buffer_to_mesh(sample_mesh, "verts", GL_FLOAT, samples, 3, (float*)line_verts, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(sample_mesh, samples, line_indices, GL_STATIC_DRAW);
	unbind_mesh_from_gl(sample_mesh);
	return sample_mesh;
}

float T[] = { 0, 1, 2, 3, 4, 5, 6 };

int main(int argc, char **argv) 
{
	startup_cgl("blub", 3, 3, argc, argv, 500, 500, true);
	register_display_function(display);
	register_idle_function(display);

	MakeTriangleNew();
	check_for_gl_errors("after trigen");
	make_shaders();

	vec3f cam_pos, cam_dir, cam_up;
	make_vec3f(&cam_pos, 0, 0, 0);
	make_vec3f(&cam_dir, 0, 0, -1);
	make_vec3f(&cam_up, 0, 1, 0);
	use_camera(make_perspective_cam("my cam", &cam_pos, &cam_dir, &cam_up, 45, 1, 0.1, 1000));
// 	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, (int)2*M_PI*100, 0, 100, 0, 0.1, 1000));
	float ortho_r = 7, ortho_l = -1,
	      ortho_t = 4, ortho_b = -1;
	use_camera(make_orthographic_cam("my ortho", &cam_pos, &cam_dir, &cam_up, ortho_r, ortho_l, ortho_t, ortho_b, 0.01, 1000));

	for (int k = 0; k < 4; ++k)
		for (int i = 0; i < 7-k; ++i)
			line_mesh[k][i] = make_sampled_bspline_mesh(500, k, i, T, 0, 6);

	vec3f *ortho_verts = malloc(sizeof(vec3f)*3*4);
	unsigned int *ortho_ids = malloc(sizeof(unsigned int) * 12);
	
	float z = 10;
	make_vec3f(ortho_verts+0, ortho_r, ortho_t, z);
	make_vec3f(ortho_verts+1, ortho_r, ortho_t-1, z);
	make_vec3f(ortho_verts+2, ortho_r-1, ortho_t, z);
	
	make_vec3f(ortho_verts+3, ortho_r, ortho_b, z);
	make_vec3f(ortho_verts+4, ortho_r, ortho_b+1, z);
	make_vec3f(ortho_verts+5, ortho_r-1, ortho_b, z);
	
	make_vec3f(ortho_verts+6, ortho_l, ortho_b, z);
	make_vec3f(ortho_verts+7, ortho_l, ortho_b+1, z);
	make_vec3f(ortho_verts+8, ortho_l+1, ortho_b, z);

	make_vec3f(ortho_verts+9,  ortho_l, ortho_t, z);
	make_vec3f(ortho_verts+10, ortho_l, ortho_t-1, z);
	make_vec3f(ortho_verts+11, ortho_l+1, ortho_t, z);

	for (int i = 0; i < 12; ++i)
		ortho_ids[i] = i;

	ortho_test = make_mesh("test", 1);
	bind_mesh_to_gl(ortho_test);
	add_vertex_buffer_to_mesh(ortho_test, "ot", GL_FLOAT, 12, 3, (float*)ortho_verts, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(ortho_test, 12, ortho_ids, GL_STATIC_DRAW);
	unbind_mesh_from_gl(ortho_test);


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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	enter_glut_main_loop();

	return 0;
}
