
//***************************************************************************
// GAME2012_A3_Shah_HardikDipakbhai.cpp by Hardik Dipakbhai Shah (C) 2020 All Rights Reserved.
//
// Assignment 3 submission.
//
// Description:
// Click run to see the results.
//
//*****************************************************************************



#include <iostream>
#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <array>
using namespace std;

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define XYZ_AXIS glm::vec3(1,1,1)

using namespace std;

// IDs.
GLuint vao, ibo, points_vbo, colors_vbo, modelID;

// Fixed timestep variables.
int deltaTime, currentTime, lastTime = 0;

// Matrices.
glm::mat4 mvp, view, projection;

float rotAngle = 0.0f;

// Mouse variables.
bool mouseFirst = true, mouseClicked = false;
int lastX, lastY;

// Texture variables.
GLuint textureID;
GLint width, height, bitDepth;

GLshort cube_indices[] = {
	// Front.
	2, 1, 0,
	0, 3, 2,
	// Left. Reverse.
	3, 0, 12,
	12, 13, 3,
	// Bottom. Reverse.
	0, 1, 9,
	9, 8, 0,
	// Right. 
	1, 2, 6,
	6, 5, 1,
	// Back. Reverse.
	5, 6, 7,
	7, 4, 5,
	// Top.
	11, 2, 3,
	3, 10, 11
};

GLfloat cube_vertices[] = {
	-0.9f, -0.9f, 0.9f,	0.25f, 0.33f,	// 0.
	0.9f, -0.9f, 0.9f, 0.5f, 0.33f,		// 1.
	0.9f, 0.9f, 0.9f, 0.5f, 0.66f,		// 2.
	-0.9f, 0.9f, 0.9f, 0.25f, 0.66f, 	// 3.
	-0.9f, -0.9f, -0.9f, 1.0f, 0.33f,	// 4.
	0.9f, -0.9f, -0.9f,	0.75f, 0.33f,	// 5.
	0.9f, 0.9f, -0.9f, 0.75f, 0.66f, 	// 6.
	-0.9f, 0.9f, -0.9f,	1.0f, 0.66f,	// 7.

	-0.9f, -0.9f, -0.9f, 0.25f, 0.0f,	// 4b. 8
	0.9f, -0.9f, -0.9f, 0.5f, 0.0f,		// 5b. 9
	-0.9f, 0.9f, -0.9f, 0.25f, 1.0f,	// 7b. 10
	0.9f, 0.9f, -0.9f, 0.5f, 1.0f,		// 6b. 11
	-0.9f, -0.9f, -0.9f, 0.0f, 0.33f,	// 4c. 12
	-0.9f, 0.9f, -0.9f, 0.0f, 0.66f		// 7c. 13
};

GLfloat colors[] = {
	1.1f, 1.0f, 1.0f,
	1.1f, 1.0f, 1.0f,
	1.1f, 1.0f, 1.0f,
	1.1f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

// Globals.
static int isWire = 0; // Is wireframe?

void loadTexture()
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load("Media/d6.png", &width, &height, &bitDepth, 0);
	if (!image) {
		cout << "Unable to load file!" << endl;
		// Could add a return too if you modify init.
	}

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up. But we don't want to unbind the texture or we cannot use it.
	stbi_image_free(image);
}

void createBuffer()
{
	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cube_vertices[0]) * 5, 0);
	glEnableVertexAttribArray(0);

	// Now for the UV/ST values.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(cube_vertices[0]) * 5, (void*)(sizeof(cube_vertices[0]) * 3));
	glEnableVertexAttribArray(2);

	colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

		// Enable depth test.
	glEnable(GL_DEPTH_TEST);
	// Enable face culling.
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
}

void init(void)
{
	vertexShaderId = setShader((char*)"vertex", (char*)"triangles.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"triangles.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "mvp");

	loadTexture();

	createBuffer();

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.5, 0.5, 1.0); // Different than Black background
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	mvp = projection * view * Model;
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &mvp[0][0]);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	// Delta time stuff.
	currentTime = glutGet(GLUT_ELAPSED_TIME); // Gets elapsed time in milliseconds.
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);

	view = glm::lookAt(
		glm::vec3(0, 5.0f, 5.0f), // Camera is at (0,3,3), in World Space,in last when 2 cubes are placed (0,5,5) in world space
		glm::vec3(0, 0, 0),	 // and looks at the origin
		glm::vec3(0, 1, 0));   // Head is up (set to 0,-1,0 to look upside-down)

	// Update the projection or view if perspective.
	projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	//calculateView();
	transformObject(0.4f, YZ_AXIS, -(rotAngle -= 4), glm::vec3(3.0f, 0.0f, 0.0f));
	//transformObject(0.4f, YZ_AXIS, rotAngle = 0, glm::vec3(0.0f, 0.0f, 0.0f));
	//transformObject(0.4f, Y_AXIS, rotAngle -=4 , glm::vec3(0.0f, 0.0f, 0.0f));
	//Ordering the GPU to start the pipeline
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glBindVertexArray(vao);

	transformObject(0.4f, YZ_AXIS, rotAngle -= 4, glm::vec3(-3.0f, 0.0f, 0.0f));

	//Ordering the GPU to start the pipeline
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle()
{
	//glutPostRedisplay();
}

void timer(int id)
{
	glutPostRedisplay();
	glutTimerFunc(33, timer, 0);
}

void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &textureID);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GAME2012_A3_Shah_HardikDipakbhai");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();

	// Set all our glut functions.
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutTimerFunc(33, timer, 0);
	atexit(clean); // This GLUT function calls specified function before terminating program. Useful!
	glutMainLoop();
}
