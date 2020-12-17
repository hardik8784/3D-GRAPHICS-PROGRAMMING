//***************************************************************************
// GAME2012_Final_ShahHardikDipakbhai.cpp by Your Name (C) 2020 All Rights Reserved.
//
// Final submission.
//
// Description:
// Click run to see the results.
// Reach to the moving Cube into the Maze!!!
//*****************************************************************************


#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "PrepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <iostream>
#include "Shape.h"
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)

float rotAngle = 0.0f;
int deltaTime, currentTime, lastTime = 0;

enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or 1 or 01
	KEY_BACKWARD = 0b00000010,		// 0x02 or 2 or 02
	KEY_LEFT = 0b00000100,
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000
	// Any other keys you want to add.
};

// IDs.
GLuint vao, ibo, points_vbo, colors_vbo, uv_vbo, normals_vbo, modelID, viewID, projID;
GLuint program, vertexShaderId,
fragmentShaderId;;

// Matrices.
glm::mat4 View, Projection;

// Our bitflags. 1 byte for up to 8 keys.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Texture variables.
//GLuint gridTexture, prismTexture, cubeTexture, coneTexture,insidecubeTexture,planeTexture;
GLuint gridTexture, backcubeTexture,frontleftcubeTexture,leftsideCubeTexture,rightsideCubeTexture,frontrightcubeTexture,middlemazeTexture,Texture2,Texture3,
		Texture4,Texture5, Texture6, Texture7, Texture8, Texture9, Texture10, 
		Texture11, Texture12, Texture13, Texture14, Texture15, Texture16,
		PrismRightFrontTexture,ConeRightFrontTexture,PrismRightRearTexture,ConeRightRearTexture,
		insidecubeTexture,PrismLeftFrontTexture,ConeLeftFrontTexture,PrismLeftRearTexture,ConeLeftRearTexture;
GLint width, height, bitDepth;

// Light variables.
AmbientLight aLight(glm::vec3(1.0f, 1.0f, 1.0f),	// Ambient colour.
	0.4f);							// Ambient strength.

DirectionalLight dLight(glm::vec3(-1.0f, 0.0f, -0.5f), // Direction.
	glm::vec3(1.0f, 1.0f, 1.0f),  // Diffuse colour.
	1.0f);						  // Diffuse strength.

PointLight pLights[] = { {glm::vec3(5.0f, 10.0f, -5.0f), 5.0f, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f } };

SpotLight sLight(glm::vec3(5.0f, 1.75f, -5.0f),	// Position.
	glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
	1.0f,							// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f),  // Direction.
	25.0f);

void timer(int);

void resetView()
{
	position = glm::vec3(5.0f, 3.0f, 10.0f);
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

// Shapes. Recommend putting in a map
//Cube g_cube;
//Cube g_insidecube;
//Prism g_prism(24);
Grid g_grid(20, 3);
//Cone g_cone(5);
//Plane g_plane();
Cube g_backsideCube, g_frontleftsideCube, g_leftSideCube, g_rightSideCube, g_frontrightsideCube, g_middlemazeCube, g_insidecube;
//Cube g_frontleftsideCube;
//Cube g_leftSideCube;
//Cube g_rightSideCube;
//Cube g_frontrightsideCube;
//Cube g_middlemazeCube;
Cube g_Texture2, g_Texture3, g_Texture4, g_Texture5, g_Texture6, g_Texture7, g_Texture8, g_Texture9, g_Texture10, g_Texture11, g_Texture12, g_Texture13, g_Texture14, g_Texture15, g_Texture16;
//Cube g_Texture3;
//Cube g_Texture4;
//Cube g_Texture5;
//Cube g_Texture6;
//Cube g_Texture7;
//Cube g_Texture8;
//Cube g_Texture9;
//Cube g_Texture10;
//Cube g_Texture11;
//Cube g_Texture12;
//ube g_Texture13;
//Cube g_Texture14;
//Cube g_Texture15;
Prism g_RightFrontPrism(12),g_RightRearPrism(12),g_LeftFrontPrism(12),g_LeftRearPrism(12);
Cone g_RightFrontCone(6),g_RightRearCone(6),g_LeftFrontCone(6),g_LeftRearCone(6);

void createTexture()
{
	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load("Media/dirt.png", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &gridTexture);
	glBindTexture(GL_TEXTURE_2D, gridTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	unsigned char* image2 = stbi_load("Media/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &backcubeTexture);
	glBindTexture(GL_TEXTURE_2D, backcubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image2);

	unsigned char* image3 = stbi_load("Media/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &frontleftcubeTexture);
	glBindTexture(GL_TEXTURE_2D, frontleftcubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image3);

	unsigned char* image4 = stbi_load("Media/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &leftsideCubeTexture);
	glBindTexture(GL_TEXTURE_2D, leftsideCubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image4);

	unsigned char* image5 = stbi_load("Media/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &rightsideCubeTexture);
	glBindTexture(GL_TEXTURE_2D, rightsideCubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image5);

	unsigned char* image6 = stbi_load("Media/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &frontrightcubeTexture);
	glBindTexture(GL_TEXTURE_2D, frontrightcubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image6);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image6);

	unsigned char* image7 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &middlemazeTexture);
	glBindTexture(GL_TEXTURE_2D, middlemazeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image7);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image7);

	unsigned char* image8 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture2);
	glBindTexture(GL_TEXTURE_2D, Texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image8);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image8);

	unsigned char* image9 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture3);
	glBindTexture(GL_TEXTURE_2D, Texture3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image9);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image9);

	unsigned char* image10 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture4);
	glBindTexture(GL_TEXTURE_2D, Texture4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image10);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image10);

	unsigned char* image11 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture5);
	glBindTexture(GL_TEXTURE_2D, Texture5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image11);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image11);

	unsigned char* image12 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture6);
	glBindTexture(GL_TEXTURE_2D, Texture6);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image12);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image12);

	unsigned char* image13 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture7);
	glBindTexture(GL_TEXTURE_2D, Texture7);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image13);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image13);

	unsigned char* image14 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture8);
	glBindTexture(GL_TEXTURE_2D, Texture8);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image14);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image14);

	unsigned char* image15 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture9);
	glBindTexture(GL_TEXTURE_2D, Texture9);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image15);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image15);

	unsigned char* image16 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture10);
	glBindTexture(GL_TEXTURE_2D, Texture10);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image16);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image16);

	unsigned char* image17 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture11);
	glBindTexture(GL_TEXTURE_2D, Texture11);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image17);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image17);

	unsigned char* image18 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture12);
	glBindTexture(GL_TEXTURE_2D, Texture12);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image18);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image18);

	unsigned char* image19 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture13);
	glBindTexture(GL_TEXTURE_2D, Texture13);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image19);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image19);

	unsigned char* image20 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture14);
	glBindTexture(GL_TEXTURE_2D, Texture14);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image20);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image20);

	unsigned char* image21 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture15);
	glBindTexture(GL_TEXTURE_2D, Texture15);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image21);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image21);

	unsigned char* image22 = stbi_load("Media/wicker_256.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &PrismRightFrontTexture);
	glBindTexture(GL_TEXTURE_2D, PrismRightFrontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image22);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image22);

	unsigned char* image23 = stbi_load("Media/cube.jpg", &width, &height, &bitDepth, 0);
	if (!image4) cout << "Unable to load file!" << endl;

	glGenTextures(1, &insidecubeTexture);
	glBindTexture(GL_TEXTURE_2D, insidecubeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image23);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image23);

	unsigned char* image24 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &Texture16);
	glBindTexture(GL_TEXTURE_2D, Texture16);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image24);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image24);

	unsigned char* image25 = stbi_load("Media/terr_rock6.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &ConeRightFrontTexture);
	glBindTexture(GL_TEXTURE_2D, ConeRightFrontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image25);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image25);


	unsigned char* image26 = stbi_load("Media/wicker_256.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &PrismRightRearTexture);
	glBindTexture(GL_TEXTURE_2D, PrismRightRearTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image26);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image26);

	unsigned char* image27 = stbi_load("Media/terr_rock6.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &ConeLeftFrontTexture);
	glBindTexture(GL_TEXTURE_2D, ConeLeftFrontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image27);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image27);

	unsigned char* image28 = stbi_load("Media/wicker_256.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &PrismLeftFrontTexture);
	glBindTexture(GL_TEXTURE_2D, PrismLeftFrontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image28);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image28);

	unsigned char* image29 = stbi_load("Media/terr_rock6.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &ConeLeftRearTexture);
	glBindTexture(GL_TEXTURE_2D, ConeLeftRearTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image29);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image29);


	unsigned char* image30 = stbi_load("Media/wicker_256.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &PrismLeftRearTexture);
	glBindTexture(GL_TEXTURE_2D, PrismLeftRearTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image30);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image30);
	//unsigned char* image3 = stbi_load("Media/noimage.jpg", &width, &height, &bitDepth, 0);
	//if (!image3) cout << "Unable to load file!" << endl;

	//glGenTextures(1, &prismTexture);
	//glBindTexture(GL_TEXTURE_2D, prismTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);
	////glBindTexture(GL_TEXTURE_2D, 0);
	//stbi_image_free(image3);


	//unsigned char* image4 = stbi_load("Media/cube.jpg", &width, &height, &bitDepth, 0);
	//if (!image4) cout << "Unable to load file!" << endl;

	//glGenTextures(1, &insidecubeTexture);
	//glBindTexture(GL_TEXTURE_2D, insidecubeTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
	//// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);
	////glBindTexture(GL_TEXTURE_2D, 0);
	//stbi_image_free(image4);

	//unsigned char* image5 = stbi_load("Media/grass.bmp", &width, &height, &bitDepth, 0);
	//if (!image5) cout << "Unable to load file!" << endl;

	//glGenTextures(1, &planeTexture);
	//glBindTexture(GL_TEXTURE_2D, planeTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	//// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);
	////glBindTexture(GL_TEXTURE_2D, 0);
	//stbi_image_free(image5);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);


}


void setupLight()
{

	// Setting ambient Light.
	glUniform3f(glGetUniformLocation(program, "aLight.ambientColour"), aLight.ambientColour.x, aLight.ambientColour.y, aLight.ambientColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.ambientStrength"), aLight.ambientStrength);

	// Setting directional light.
	glUniform3f(glGetUniformLocation(program, "dLight.base.diffuseColour"), dLight.diffuseColour.x, dLight.diffuseColour.y, dLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "dLight.base.diffuseStrength"), dLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "dLight.direction"), dLight.direction.x, dLight.direction.y, dLight.direction.z);

	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColour"), pLights[0].diffuseColour.x, pLights[0].diffuseColour.y, pLights[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].exponent"), pLights[0].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColour"), pLights[1].diffuseColour.x, pLights[1].diffuseColour.y, pLights[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[1].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[1].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[1].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].exponent"), pLights[1].exponent);

	// Setting spot light.
	glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColour"), sLight.diffuseColour.x, sLight.diffuseColour.y, sLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);
}

void setupBuffer()
{
	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	ibo = 0;
	glGenBuffers(1, &ibo);

	points_vbo = 0;
	glGenBuffers(1, &points_vbo);

	colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);

	uv_vbo = 0;
	glGenBuffers(1, &uv_vbo);

	normals_vbo = 0;
	glGenBuffers(1, &normals_vbo);

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

}

void init(void)
{
	srand((unsigned)time(NULL));
	//Specifying the name of vertex and fragment shaders.
	vertexShaderId = setShader((char*)"vertex", (char*)"spotlight.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"spotlight.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "model");
	projID = glGetUniformLocation(program, "projection");
	viewID = glGetUniformLocation(program, "view");

	// Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	setupBuffer();
	setupLight();
	createTexture();

	// Change shape data.
	//g_prism.SetMat(0.1, 16);
	g_grid.SetMat(0.0, 16);

	// Enable depth test and blend.
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ZERO);

	//we want to take the alpha of the source color vector for the source factor and 1−alpha of the same color vector for the destination factor.
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//It is also possible to set different options for the RGB and alpha channel individually
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,   GL_ONE, GL_ZERO );

	//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	//glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);

	//pick one of the blend function
	//glBlendColor(1.0, 0.0, 0.0, 1.0);
	////glBlendFunc(GL_CONSTANT_COLOR, GL_DST_COLOR);
	//glBlendFunc( GL_SRC_COLOR, GL_CONSTANT_COLOR);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_SRC_ALPHA_SATURATE);


	//Src+Dst
	glBlendEquation(GL_FUNC_ADD);

	//Src−Dst
	//glBlendEquation(GL_FUNC_SUBTRACT);

	//Dst−Src
	//glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

	//min(Dst, Src).
	//glBlendEquation(GL_MIN); 

	 //max(Dst,Src).
	//glBlendEquation(GL_MAX); 



	// Enable face culling.
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	timer(0);


	//Main Here.....................................
	//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	////Src+Dst
	//glBlendEquation(GL_FUNC_ADD);

	//glClearColor(0.1f, 0.1f, 0.199f, 1.0f); // Different than Black background

	//timer(0);
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
	glUniform3f(glGetUniformLocation(program, "eyePosition"), position.x, position.y, position.z);
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);

	calculateView();
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

void drawGrid()
{
	glBindTexture(GL_TEXTURE_2D, gridTexture);
	g_grid.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_grid.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawBacksideCube()
{
	glBindTexture(GL_TEXTURE_2D, backcubeTexture);
	g_backsideCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(20.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -20.0f));
	glDrawElements(GL_TRIANGLES, g_backsideCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}


void drawfrontleftCube()
{
	glBindTexture(GL_TEXTURE_2D, frontleftcubeTexture);
	g_frontleftsideCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(9.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_frontleftsideCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawleftCube()
{
	glBindTexture(GL_TEXTURE_2D, leftsideCubeTexture);
	g_leftSideCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(20.0f, 1.0f, -1.0f), Y_AXIS, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_leftSideCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}


void drawrightCube()
{
	glBindTexture(GL_TEXTURE_2D, rightsideCubeTexture);
	g_rightSideCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(20.0f, 1.0f, -1.0f), Y_AXIS, 90.0f, glm::vec3(20.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_rightSideCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawfrontrightCube()
{
	glBindTexture(GL_TEXTURE_2D, frontrightcubeTexture);
	g_frontrightsideCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(9.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(10.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_frontrightsideCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//1
void drawmiddlemazeCube()
{
	glBindTexture(GL_TEXTURE_2D, middlemazeTexture);
	g_middlemazeCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(10.0f, 0.0f, -10.0f));
	glDrawElements(GL_TRIANGLES, g_middlemazeCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}
//2
void drawTexture2()
{
	glBindTexture(GL_TEXTURE_2D, Texture2);
	g_Texture2.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(8.0f, 0.0f, -8.0f));
	glDrawElements(GL_TRIANGLES, g_Texture2.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//3
void drawTexture3()
{
	glBindTexture(GL_TEXTURE_2D, Texture3);
	g_Texture3.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(8.0f, 0.0f, -6.0f));
	glDrawElements(GL_TRIANGLES, g_Texture3.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//4
void drawTexture4()
{
	glBindTexture(GL_TEXTURE_2D, Texture4);
	g_Texture4.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(11.0f, 0.0f, -5.0f));
	glDrawElements(GL_TRIANGLES, g_Texture4.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//5
void drawTexture5()
{
	glBindTexture(GL_TEXTURE_2D, Texture5);
	g_Texture5.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(5.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(11.0f, 0.0f, -7.0f));
	glDrawElements(GL_TRIANGLES, g_Texture5.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//6
void drawTexture6()
{
	glBindTexture(GL_TEXTURE_2D, Texture6);
	g_Texture6.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(9.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(7.0f, 0.0f, -13.0f));
	glDrawElements(GL_TRIANGLES, g_Texture6.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//7
void drawTexture7()
{
	glBindTexture(GL_TEXTURE_2D, Texture7);
	g_Texture7.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(10.0f, 1.0f, -1.0f), Y_AXIS, 90.0f, glm::vec3(7.0f, 0.0f, -4.0f));
	glDrawElements(GL_TRIANGLES, g_Texture7.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//8
void drawTexture8()
{
	glBindTexture(GL_TEXTURE_2D, Texture8);
	g_Texture8.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(6.0f, 1.0f, -1.0f), Y_AXIS, -90.0f, glm::vec3(15.0f, 0.0f, -13.0f));
	glDrawElements(GL_TRIANGLES, g_Texture8.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//9
void drawTexture9()
{
	glBindTexture(GL_TEXTURE_2D, Texture9);
	g_Texture9.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(2.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(12.0f, 0.0f, -11.0f));
	glDrawElements(GL_TRIANGLES, g_Texture9.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//10
void drawTexture10()
{
	glBindTexture(GL_TEXTURE_2D, Texture10);
	g_Texture10.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(13.0f, 0.0f, -6.0f));
	glDrawElements(GL_TRIANGLES, g_Texture10.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//11
void drawTexture11()
{
	glBindTexture(GL_TEXTURE_2D, Texture11);
	g_Texture11.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(2.0f, 1.0f, -2.0f), Y_AXIS, -90.0f, glm::vec3(12.0f, 0.0f, -10.0f));
	glDrawElements(GL_TRIANGLES, g_Texture11.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//12
void drawTexture12()
{
	glBindTexture(GL_TEXTURE_2D, Texture12);
	g_Texture12.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(13.0f, 1.0f, -1.0f), Y_AXIS, -90.0f, glm::vec3(3.0f, 0.0f, -16.0f));
	glDrawElements(GL_TRIANGLES, g_Texture12.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//13
void drawTexture13()
{
	glBindTexture(GL_TEXTURE_2D, Texture13);
	g_Texture13.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(14.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(3.0f, 0.0f, -16.0f));
	glDrawElements(GL_TRIANGLES, g_Texture13.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//14
void drawTexture14()
{
	glBindTexture(GL_TEXTURE_2D, Texture14);
	g_Texture14.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(14.0f, 1.0f, -1.0f), Y_AXIS, -90.0f, glm::vec3(17.0f, 0.0f, -17.0f));
	glDrawElements(GL_TRIANGLES, g_Texture14.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//15
void drawTexture15()
{
	glBindTexture(GL_TEXTURE_2D, Texture15);
	g_Texture15.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(9.0f, 1.0f, -1.0f), X_AXIS, 0.0f, glm::vec3(6.0f, 0.0f, -3.0f));
	glDrawElements(GL_TRIANGLES, g_Texture15.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawRightFrontPrism()
{
	glBindTexture(GL_TEXTURE_2D, PrismRightFrontTexture);
	g_RightFrontPrism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_RightFrontPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawRightFrontCone()
{
	glBindTexture(GL_TEXTURE_2D, ConeRightFrontTexture);
	g_RightFrontCone.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_RightFrontPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawRightRearPrism()
{
	glBindTexture(GL_TEXTURE_2D, PrismRightFrontTexture);
	g_RightFrontPrism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_RightFrontPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawRightRearCone()
{
	glBindTexture(GL_TEXTURE_2D, ConeRightFrontTexture);
	g_RightFrontCone.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_RightFrontPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawLeftFrontPrism()
{
	glBindTexture(GL_TEXTURE_2D, PrismLeftFrontTexture);
	g_LeftFrontPrism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_LeftFrontPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawLeftFrontCone()
{
	glBindTexture(GL_TEXTURE_2D, ConeLeftFrontTexture);
	g_LeftFrontCone.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 2.0f, -1.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_LeftFrontCone.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawLeftRearPrism()
{
	glBindTexture(GL_TEXTURE_2D, PrismLeftRearTexture);
	g_LeftRearPrism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -21.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 0.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_LeftRearPrism.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawLeftRearCone()
{
	glBindTexture(GL_TEXTURE_2D, ConeLeftRearTexture);
	g_LeftRearCone.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -1.0f));
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 2.0f, -21.0f));
	//transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(19.0f, 2.0f, -21.0f));
	glDrawElements(GL_TRIANGLES, g_LeftRearCone.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void drawinsideCube()
{
	glBindTexture(GL_TEXTURE_2D, insidecubeTexture);
	g_insidecube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(0.2f, 0.2f, 0.2f), X_AXIS, rotAngle += ((float)45 / (float)1000 * deltaTime), glm::vec3(14.5f, 0.5f, -8.5f));
	glDrawElements(GL_TRIANGLES, g_insidecube.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//16
void drawTexture16()
{
	glBindTexture(GL_TEXTURE_2D, Texture16);
	g_Texture16.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(13.0f, 1.0f, -0.5f), Y_AXIS, -90.0f, glm::vec3(4.8f, 0.0f, -15.0f));
	glDrawElements(GL_TRIANGLES, g_Texture16.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

//void drawPrism()
//{
//
//	glBindTexture(GL_TEXTURE_2D, prismTexture);
//	g_prism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
//	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(5.0f, 0.0f, -5.0f));
//	glDrawElements(GL_TRIANGLES, g_prism.NumIndices(), GL_UNSIGNED_SHORT, 0);
//}
//
//void drawPlane()
//{
//	glBindTexture(GL_TEXTURE_2D, planeTexture);
//	g_prism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
//	transformObject(glm::vec3(1.0f, 1.0f, 0.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
//	glDrawElements(GL_TRIANGLES, g_plane.NumIndices(), GL_UNSIGNED_SHORT, 0);
//}
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
	// Draw all shapes.

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	//drawPrism();
	drawinsideCube();
	//drawCube();
	drawGrid();
	drawBacksideCube();
	drawfrontleftCube();
	//drawCube();
	drawleftCube();
	drawrightCube();
	drawfrontrightCube();
	drawmiddlemazeCube();
	drawTexture2();
	drawTexture3();
	drawTexture4();
	drawTexture5();
	drawTexture6();
	drawTexture7();
	drawTexture8();
	drawTexture9();
	drawTexture10();
	drawTexture11();
	drawTexture12();
	drawTexture13();
	drawTexture14();
	drawTexture15();
	drawTexture16();
	drawRightFrontPrism();
	drawRightFrontCone();
	drawRightRearPrism();
	drawRightRearCone();
	drawLeftFrontPrism();
	drawLeftFrontCone();
	drawLeftRearPrism();
	drawLeftRearCone();

	glBindVertexArray(0); // Done writing.
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	else if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	else if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position.y += MOVESPEED;
	else if (keys & KEY_DOWN)
		position.y -= MOVESPEED;
}

void timer(int) { // essentially our update()
	parseKeys();
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, 0); // 60 FPS or 16.67ms.
}

//---------------------------------------------------------------------
//
// keyDown
//
void keyDown(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD; break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT; break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT; break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP; break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN; break;
	case 'i':
		sLight.position.z -= 0.1; break;
	case 'j':
		sLight.position.x -= 0.1; break;
	case 'k':
		sLight.position.z += 0.1; break;
	case 'l':
		sLight.position.x += 0.1; break;
	case 'p':
		sLight.position.y += 0.1; break;
	case ';':
		sLight.position.y -= 0.1; break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	if (key == GLUT_KEY_UP)
	{
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; break;
	case 's':
		keys &= ~KEY_BACKWARD; break;
	case 'a':
		keys &= ~KEY_LEFT; break;
	case 'd':
		keys &= ~KEY_RIGHT; break;
	case 'r':
		keys &= ~KEY_UP; break;
	case 'f':
		keys &= ~KEY_DOWN; break;
	case ' ':
		resetView();
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	if (key == GLUT_KEY_UP)
	{
		keys &= ~KEY_FORWARD;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		keys &= ~KEY_BACKWARD;
	}
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &gridTexture);
	//glDeleteTextures(1, &prismTexture);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GAME2012_Final_ShahHardikDipakbhai");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp); // New function for third example.
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This GLUT function calls specified function before terminating program. Useful!

	glutMainLoop();

	return 0;
}

