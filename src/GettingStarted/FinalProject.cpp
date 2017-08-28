// Author: Stewart Charles
// Assignment 5
// Final Project: Volume Renderer

#include <sb7.h>
#include <string>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>
#include <vector>
#include "Cube.h"

#define PI 3.14159265
using namespace std;
using namespace vmath;

const int BOX_RES = 1024;

// Debugging functions
void printShaderInfoLog(GLuint obj) {
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		OutputDebugString(infoLog);
		free(infoLog);
	}
}

// Custom matrix vector multiplication function
static inline vec4 mult_matrix_vec4(mat4 m, vec4 v) {
	auto answer = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i<4; i++) {
		for (int j = 0; j<4; j++) {
			answer[i] += (m[i][j] * v[j]);
		}
	}
	return answer;
}

void printProgramInfoLog(GLuint obj) {
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0) {
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		OutputDebugString(infoLog);
		free(infoLog);
	}
}

/* Avoid use of vmath::mix function, and instead use this implementation.
It seems that the 'sign' of which vector is mixed is switched.
*/
static vec4 mix(vec4 a, vec4 b, GLfloat t) {
	auto minus = 1.0 - t;
	return (a * minus + b * t);
}

class gettingStarted_app : public sb7::application {

protected:
	void init() {
		static const char title[] = "Final Project: Volume Renderer";
		sb7::application::init();
		memcpy(info.title, title, sizeof(title));
		info.windowWidth = 512;
		info.windowHeight = 512;
	}

	void startup();
	void render(double currentTime);
	void onKey(int key, int action);
	void onMouseMove(int x, int y);
	void onMouseButton(int button, int action);
	vec3 getArcballVector(int x, int y);

	GLuint viewray_shader;
	GLuint volume_shader;

	struct uniforms_block {
		mat4 view_matrix;
		mat4 proj_matrix;
	};
	GLuint	uniforms_buffer;
	
	// Uniform Locations
	GLint mv_loc;
	GLint lightpos_loc;
	GLint view_loc;
	GLint shadow_mat_loc;

	// Variables for mouse interaction
	bool bPerVertex;
	bool bShiftPressed = false;
	bool bZoom = false;
	bool bRotate = false;
	bool bPan = false;

	int iWidth = info.windowWidth;
	int iHeight = info.windowHeight;

	// Rotation and Translation matricies for moving the camera by mouse interaction.
	mat4 rotationMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
private:
	// Variables for mouse position to solve the arcball vectors
	int iPrevMouseX = 0;
	int iPrevMouseY = 0;
	int iCurMouseX = 0;
	int iCurMouseY = 0;

	// Scale of the objects in the scene
	float fScale = 7.0f;
	float step_size = 0.005;
	bool first_hit = true;
	// Initial position of the camera
	float fXpos = 0.0f;
	float fYpos = 0.0f;
	float fZpos = 75.0f;

	GLuint buffers[1];
	GLuint vao;
	GLuint rbo;
	GLuint textures[4];
	GLuint fbuffers[2];

	Cube cubes[1];
};

void gettingStarted_app::startup() {
	glGenFramebuffers(2, fbuffers);
	glGenTextures(4, textures);

	// Calculating volume intersection rays using Cube samples
	// Only need cube verts
	auto vertex_data = Cube::get_verts();

	// Setup Vertex Attribute Buffers
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, buffers);

	// Not sure if this is needed, but I'm reluctant to remove it after everything works...
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, BOX_RES,BOX_RES);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// ---- Setup Texture Buffers

	/* Load 'raw' data from .dat file
	   Technique derived from https://www.cg.tuwien.ac.at/research/vis/datasets/#Data%20sets */
	auto LoadVolumeFromFile = [&](const char* fileName, int index) {
		FILE *fp = fopen(fileName, "rb");

		// Retrieve dimensions of file from header
		unsigned short vuSize[3];
		fread((void*)vuSize, 3, sizeof(unsigned short), fp);
		int uCount = int(vuSize[0])*int(vuSize[1])*int(vuSize[2]);
		unsigned short *pData = new unsigned short[uCount];
		fread((void*)pData, uCount, sizeof(unsigned short), fp);

		fclose(fp);

		/* Normalize the scalar values from 0 to 1 for easy texture lookup */
		vector<GLfloat> texture_sample;
		for (int i = 0; i < uCount; i++)
			texture_sample.push_back( pData[i] / 4095.0);

		/* Load data into a 3D texture 
		   Take care that the 'type' of the pixel data is correct, otherwise
		   you get unaligned bits and strange things happen. */
		glBindTexture(GL_TEXTURE_3D, textures[index]);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED,
			int(vuSize[0]), int(vuSize[1]), int(vuSize[2]), 0, GL_RED, GL_FLOAT, texture_sample.data());

		/* Produces awesome results
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB,
			int(vuSize[0]), int(vuSize[1]), int(vuSize[2]), 0, GL_RGB, GL_UNSIGNED_BYTE, texture_sample.data()); */

		// Are these correct texture parameters???
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete [] pData;
	};

	/* Function that Loads the .1dt transfer function data that I generated from  
	   ImageVis3D. The transfer function data itself is just a linear array of RGBA values
	   and a header that defines the number of RGBA colors in the file. */
	auto LoadTransferFunction = [&](const char* fileName) {
		FILE * file = fopen(fileName, "r");
		vector<GLfloat> transfer_fn;
		// There's probably better ways to read this file...
		bool first_line = true;
		int length = INT_MAX;
		int count = 0;
		float r, g, b, a;
		while (count < length) {
			if (first_line) {
				fscanf(file, "%d", &length);
				first_line = false;
				continue;
			}
			// Read the line as RGBA float values separated by a space
			fscanf(file, "%f %f %f %f", &r, &g, &b, &a);
			auto arr = { r, g, b, a };
			transfer_fn.insert(transfer_fn.end(), arr.begin(), arr.end());
			count++;
		}
		return transfer_fn;
	};

	// Buffer @1 Texture Buffer is 3D Volume Data BUffer
	LoadVolumeFromFile("src/christmastree256x249x256.dat", 0);

	// Load the Transfuer function data into a Vector
	auto transfer_function = LoadTransferFunction("src/christmastree256x249x256.1dt");

	// Setup Transfer Function Texture
	glBindTexture(GL_TEXTURE_1D, textures[1]);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, transfer_function.size() / 4, 0, GL_RGBA, GL_FLOAT, transfer_function.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	// Setup Frame Buffer texture for 'Front' faces
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BOX_RES,BOX_RES, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	// Setup Frame Buffer for storing 'Front' faces
	glBindFramebuffer(GL_FRAMEBUFFER, fbuffers[0]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[2], 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	// Setup Frame Buffer texture for 'Rear' faces
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BOX_RES, BOX_RES, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	// Setup Frame Buffer for storing 'Rear' faces
	glBindFramebuffer(GL_FRAMEBUFFER, fbuffers[1]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[3], 0);
	DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	/* TODO: Use Central Differences to calculate normals of volume data into a 3D Texture */
	// Buffer @5 or @0 ? Store calculated Normals into 3D Texture, and store Scalar Component as alpha component for 'compactness'

	/* ---- Setup Shader Programs for
	   1) viewray_shader : Calculates front and back
	      points of intersection into Volume/Cube 
	   2) volume_shader : Performs the actual volume rendering */

	// Setup shader program for rendering front and back faces for use in calculating view rays
	viewray_shader = glCreateProgram();
	auto vr_fs = sb7::shader::load("src/viewray_shader.fs.glsl", GL_FRAGMENT_SHADER);
	glCompileShader(vr_fs);
	GLint success = 0;
	printShaderInfoLog(vr_fs);
	glGetShaderiv(vr_fs, GL_COMPILE_STATUS, &success);
	assert(success != GL_FALSE);

	auto vr_vs = sb7::shader::load("src/viewray_shader.vs.glsl", GL_VERTEX_SHADER);
	glCompileShader(vr_vs);
	printShaderInfoLog(vr_vs);
	glGetShaderiv(vr_vs, GL_COMPILE_STATUS, &success);
	assert(success != GL_FALSE);
	success = 0;

	glAttachShader(viewray_shader, vr_vs);
	glAttachShader(viewray_shader, vr_fs);

	glLinkProgram(viewray_shader);
	printProgramInfoLog(viewray_shader);
	glGetProgramiv(viewray_shader, GL_LINK_STATUS, &success);
	assert(success != GL_FALSE);
	success = 0;
		
	// Setup shader program for performing the GPU raycasting into the Volume Data itself
	volume_shader = glCreateProgram();
	auto vu_fs = sb7::shader::load("src/volume_shader.fs.glsl", GL_FRAGMENT_SHADER);
	glCompileShader(vu_fs);
	printShaderInfoLog(vu_fs);
	glGetShaderiv(vu_fs, GL_COMPILE_STATUS, &success);
	assert(success != GL_FALSE);
	success = 0;

	auto vu_vs = sb7::shader::load("src/volume_shader.vs.glsl", GL_VERTEX_SHADER);
	glCompileShader(vu_vs);
	printShaderInfoLog(vu_vs);
	glGetShaderiv(vu_vs, GL_COMPILE_STATUS, &success);
	assert(success != GL_FALSE);
	success = 0;

	glAttachShader(volume_shader, vu_vs);
	glAttachShader(volume_shader, vu_fs);

	glLinkProgram(volume_shader);
	printProgramInfoLog(volume_shader);
	glGetProgramiv(viewray_shader, GL_LINK_STATUS, &success);
	assert(success != GL_FALSE);
	success = 0;

	/* TODO: Create a transfer function state on the application side to be able to change a uniform in the Shader program
	that can be used to 'modify' the transfer function in realtime. */

	// Setup Vertex Position Attribute buffer @0
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

	cubes[0] = Cube(buffers[0]);

	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void gettingStarted_app::render(double currentTime) {
	// Set the Program to the Current desired Shader profile
	static const GLfloat ones[] = { 1.0f };
	static const GLfloat zeros[] = { 0.0f };
	static const GLfloat gray[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float f = (float)currentTime;

	// Calculations for mouse interaction camera rotation and translation matrix
	float fAngle = 0.0f;
	vec3 axis_in_camera_coord = (0.0f, 1.0f, 0.0f);

	if (iCurMouseX != iPrevMouseX || iCurMouseY != iPrevMouseY) {
		// Arcball Rotation
		if (bRotate) {
			vec3 va = getArcballVector(iPrevMouseX, iPrevMouseY);
			vec3 vb = getArcballVector(iCurMouseX, iCurMouseY);
			fAngle = acos(fmin(1.0f, dot(va, vb)));
			axis_in_camera_coord = cross(va, vb);
			axis_in_camera_coord = normalize(axis_in_camera_coord);
			iPrevMouseX = iCurMouseX;
			iPrevMouseY = iCurMouseY;
			rotationMatrix *= rotate(degrees(fAngle), axis_in_camera_coord);
		}
		// Zoom in and out
		if (bZoom) {
			fZpos += (iCurMouseY - iPrevMouseY);
			if (fZpos > 500)
				fZpos = 500;
			else if (fZpos < 10) 
				fZpos = 10;

			iPrevMouseY = iCurMouseY;
			iPrevMouseX = iCurMouseX;
		}
		// Pan camera left, right, up, and down
		if (bPan) {
			fXpos += (iCurMouseX - iPrevMouseX);
			fYpos += (iCurMouseY - iPrevMouseY);
			iPrevMouseY = iCurMouseY;
			iPrevMouseX = iCurMouseX;
			translationMatrix = translate(fXpos / (info.windowWidth / fZpos), -fYpos / (info.windowWidth / fZpos), 0.0f);
		}
	}

	// Set up view and perspective matrix
	vec3 view_position = vec3(0.0f, 0.0f, fZpos);
	mat4 view_matrix = lookat(view_position,
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	view_matrix *= translationMatrix;
	view_matrix *= rotationMatrix;
	mat4 perspective_matrix = perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

	mat4  model_matrix =
		mat4::identity();

	/* Render and store the Front and Back faces of our Volume into a Texture */
	glUseProgram(viewray_shader);
	glViewport(0, 0, BOX_RES, BOX_RES);
	mv_loc = glGetUniformLocation(viewray_shader, "mv_matrix");
	auto proj_loc = glGetUniformLocation(viewray_shader, "proj_matrix");

	model_matrix =
		scale(10.0f);
	glUniformMatrix4fv(mv_loc, 1, false, view_matrix * model_matrix);
	glUniformMatrix4fv(proj_loc, 1, false, perspective_matrix);


	// Render front face
	glEnable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbuffers[0]);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_DEPTH, 0, ones);
	glCullFace(GL_BACK);

	cubes[0].draw_cube();

	// Render back face
	glBindFramebuffer(GL_FRAMEBUFFER, fbuffers[1]);
	glViewport(0, 0, BOX_RES, BOX_RES);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_DEPTH, 0, ones);
	glCullFace(GL_FRONT);
	
	cubes[0].draw_cube();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_DEPTH, 0, ones);
	
	// Ray-cast into the Volume using the front and back position data of the Volume region
	glUseProgram(volume_shader);
	
	glCullFace(GL_BACK);
	glViewport(0, 0, info.windowWidth, info.windowHeight);
	mv_loc = glGetUniformLocation(volume_shader, "mv_matrix");
	proj_loc = glGetUniformLocation(volume_shader, "proj_matrix");
	auto vport_loc = glGetUniformLocation(volume_shader, "viewport");
	auto step_loc = glGetUniformLocation(volume_shader, "step_size");
	auto hit_loc = glGetUniformLocation(volume_shader, "first_hit");
	// Setup the Textures we will be using
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, textures[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	
	// Finally render the damn thing
	glUniformMatrix4fv(mv_loc, 1, false, view_matrix * model_matrix);
	glUniformMatrix4fv(proj_loc, 1, false, perspective_matrix);
	glUniform2fv(vport_loc, 1, vec2(info.windowWidth, info.windowHeight));
	glUniform1f(step_loc, step_size);
	glUniform1i(hit_loc, first_hit);
	cubes[0].draw_cube();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void gettingStarted_app::onKey(int key, int action) {
	// Check to see if shift was pressed
	if (action == GLFW_PRESS && (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)) {
		bShiftPressed = true;
	}
	if (action) {
		switch (key) {
		case 'R':
			rotationMatrix = mat4::identity();
			translationMatrix = mat4::identity();
			fXpos = 0.0f;
			fYpos = 0.0f;
			fZpos = 75.0f;
			break;
		case 'W' :
			if (step_size < 0.25)
				step_size *= 2.0;
			break;
		case 'S':
			if (step_size > 0.00025)
				step_size /= 2.0;
			break;
		case 'A':
			first_hit = first_hit ? false : true;
			break;
		}
	}
	// Check to see if shift was released
	if (action == GLFW_RELEASE) {
		bShiftPressed = false;
	}
}

void gettingStarted_app::onMouseButton(int button, int action) {
	int x, y;

	getMousePosition(x, y);
	// Check to see if left mouse button was pressed for rotation
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		bRotate = true;
		iPrevMouseX = iCurMouseX = x;
		iPrevMouseY = iCurMouseY = y;
	}
	// Check to see if right mouse button was pressed for zoom and pan
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		bZoom = false;
		bPan = false;
		if (bShiftPressed == true) {
			bZoom = true;
		}
		else if (bShiftPressed == false) {
			bPan = true;
		}
		iPrevMouseX = iCurMouseX = x;
		iPrevMouseY = iCurMouseY = y;
	}
	else {
		bRotate = false;
		bZoom = false;
		bPan = false;
	}
}

void gettingStarted_app::onMouseMove(int x, int y) {
	// If rotating, zooming, or panning save mouse x and y
	if (bRotate || bZoom || bPan) {
		iCurMouseX = x;
		iCurMouseY = y;
	}
}

// Modified from tutorial at the following website:
// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball

vec3 gettingStarted_app::getArcballVector(int x, int y) {
	// find vector from origin to point on sphere
	vec3 vecP = vec3(1.0f*x / info.windowWidth * 2 - 1.0f, 1.0f*y / info.windowHeight * 2 - 1.0f, 0.0f);
	// inverse y due to difference in origin location on the screen
	vecP[1] = -vecP[1];
	float vecPsquared = vecP[0] * vecP[0] + vecP[1] * vecP[1];
	// solve for vector z component
	if (vecPsquared <= 1)
		vecP[2] = sqrt(1 - vecPsquared);
	else
		vecP = normalize(vecP);
	return vecP;
}

DECLARE_MAIN(gettingStarted_app)