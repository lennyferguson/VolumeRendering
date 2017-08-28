#ifndef _CUBE_H_
#define _CUBE_H_

#include <sb7.h>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>
#include <vector>

// Helper class for constructing a cube.
// Has static helper functions for generating Vertex Attribute buffer data
struct Cube {
public:
	int vBuffer;

	Cube::Cube(){
		vBuffer = 0;
	}

	Cube::Cube(int vb_index) {
		this->vBuffer = vb_index;
	}

	void Cube::draw_cube() {
		glBindBuffer(GL_ARRAY_BUFFER, this->vBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	static std::vector<GLfloat> Cube::get_verts() {
		std::vector<GLfloat> v{
			// Cube
			//B
			-1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,

			//R
			1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f,

			//F
			1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,

			-1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,

			//L
			-1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,

			-1.0f, -1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,

			//D
			-1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f,

			-1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f, 1.0f,

			//U
			-1.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};
		return v;
	}

	static std::vector<GLfloat> Cube::get_colors() {
		std::vector<GLfloat> v{
			// Room Start
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,

			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f,
			0.0f, 0.450f, 0.80f, 1.0f
		};
		return v;
	}
	static std::vector<GLfloat> Cube::get_texture_coords() {
		std::vector<GLfloat> v{
			1.0f,1.0f,
			1.0f,0.0f,
			0.0f,1.0f,

			1.0f,0.0f,
			0.0f,0.0f,
			0.0f,1.0f,

			1.0f,1.0f,
			1.0f,0.0f,
			0.0f,1.0f,

			1.0f,0.0f,
			0.0f,0.0f,
			0.0f,1.0f,

			1.0f,1.0f,
			1.0f,0.0f,
			0.0f,1.0f,

			1.0,0.0f,
			0.0f,0.0f,
			0.0f,1.0f,

			1.0f,1.0f,
			1.0f,0.0f,
			0.0f,1.0f,

			1.0f,0.0f,
			0.0f,0.0f,
			0.0f,1.0f,

			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,

			1.0f,0.0f,
			0.0f,1.0f,
			0.0f,0.0f,

			1.0f,1.0f,
			1.0f,0.0f,
			0.0f,1.0f,

			1.0f,0.0f,
			0.0f,0.0f,
			0.0f,1.0f
		};
		return v;
	}
};

#endif