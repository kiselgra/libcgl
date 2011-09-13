#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>

#include "mesh.h"
#include "shader.h"

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
		"in vec3 in_Color;\n"
		"out vec3 ex_Color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vec4(in_Position, 1.0);\n"
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
	compile_and_link_shader(my_shader);
}


void render_tri_new()
{
	bind_shader(my_shader);

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
	switch (key) {
		case 27:
			exit(0);
			break;
	}
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

	// broken awesome on ubuntu
	display();
	display();
	// ;

	glutMainLoop();


	return 0;
}
