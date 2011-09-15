#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>

#include "mesh.h"
#include "shader.h"

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/matrix.h>
#include <libmcm-0.0.1/camera-matrices.h>

matrix4x4f projection_matrix, lookat_matrix, gl_view_matrix;
vec3f cam_pos, cam_dir, cam_up;

void check_for_errors(const char *function)
{
	GLenum error;
	bool fucked = false;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		fprintf (stderr, "GL error %d detected in %s\n", error, function);
		fucked = true;
	}

	if (fucked) exit(-1);
}

void ignore_error(const char *function)
{
	GLenum error = glGetError();
	fprintf(stderr, "Ignoring GL error %s (for %s).\n", gluErrorString(error), function);
}

void dump_info(void)
{
	check_for_errors ("pre dumpInfo");
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	check_for_errors ("dumpInfo");
}


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


GLuint vertex_shader;
GLuint fragment_shader;   
GLuint shader;


shader_ref my_shader;

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
		"uniform mat4 proj;\n"
		"uniform mat4 moview;\n"
		"in vec3 in_Color;\n"
		"out vec3 ex_Color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = proj * moview * vec4(in_Position, 1.0); //vec4(in_Position, 1.0);\n"
		"	ex_Color = in_Color;\n"
		"}\n";

	const char *frag = 
		"// minimal.frag\n"
		"#version 150 core\n"
		"\n"
		"// precision highp float;\n"
		"\n"
		"in vec3 ex_Color;\n"
		"out vec4 out_Color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	out_Color = vec4(1,0,0,1.0);\n"
		"}\n";
	

	my_shader = make_shader("my shader", 1);
	add_vertex_source(my_shader, vertex);
	add_fragment_source(my_shader, frag);
	add_shader_input(my_shader, "in_Position", 0);
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
}


void render_tri_new()
{
	bind_shader(my_shader);
	int loc = glGetUniformLocation(gl_shader_object(my_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix.col_major);
	loc = glGetUniformLocation(gl_shader_object(my_shader), "moview");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix.col_major);

	bind_mesh_to_gl(tri_mesh);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	unbind_mesh_from_gl(tri_mesh);
	
	unbind_shader(my_shader);
}

void display(void)
{
	static float c = 0;
	//c += 0.00001;
	if (c > 2.0f) c = 0;
	//glClearColor(c>1.0f ? 2.0-c : c,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_tri_new();

	glutSwapBuffers();
// 	glutPostRedisplay();
	check_for_errors("display");
}

void keyboard(unsigned char key, int x, int y)
{
	static float angle = 0;
	vec3f tmp;
	switch (key) {
		case 27:
			exit(0);
		case 'r':
			angle += 0.1;
			float x = cos(angle);
			float y = sin(angle);
			make_vec3f(&cam_pos, y, 0, x);
			make_vec3f(&tmp, 0, 0, 0);
			sub_components_vec3f(&cam_dir, &tmp, &cam_pos);
			break;

		case 'l':
			make_vec3f(&tmp, 0.02, 0, 0);
			add_components_vec3f(&cam_pos, &tmp, &cam_pos);
			break;
	}
	make_lookat_matrixf(&lookat_matrix, &cam_pos, &cam_dir, &cam_up);
	make_gl_viewing_matrixf(&gl_view_matrix, &lookat_matrix);
}


int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitContextVersion (3, 3);
	glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow("GLEW Test");
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	ignore_error("glew-init");
	
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	glViewport(0,0,500,500);
	// dumpInfo ();
	// init ();
	glutDisplayFunc(display); 
	glutIdleFunc(display); 
	// glutReshapeFunc(reshape);
	// glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	dump_info();


// 	MakeTriangle();
	MakeTriangleNew();
	check_for_errors("after trigen");
	make_shaders();

	make_projection_matrixf(&projection_matrix, 45, 1, 0.01, 10000);

	make_vec3f(&cam_pos, 0, 0, 1);
	make_vec3f(&cam_dir, 0, 0, -1);
	make_vec3f(&cam_up, 0, 1, 0);
	make_lookat_matrixf(&lookat_matrix, &cam_pos, &cam_dir, &cam_up);
	make_gl_viewing_matrixf(&gl_view_matrix, &lookat_matrix);

	// broken awesome on ubuntu
	display();
	display();
	// ;

	glutMainLoop();


	return 0;
}
