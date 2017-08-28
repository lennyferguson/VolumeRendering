#ifndef _SPHERE_H_
#define _SPHERE_H_
#include <sb7.h>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>
#include <vector>

// Structure that draws a Sphere from designated buffers
struct Sphere {
public:
	Sphere::Sphere(int vbuffer_index, int cbuffer_index, int nbuffer_index, int size) {
		this->vBuffer = vbuffer_index;
		this->cBuffer = cbuffer_index;
		this->nBuffer = nbuffer_index;
		this->sphere_size = size;
	}
	Sphere::Sphere() {
		vBuffer = 0;
		cBuffer = 0;
		nBuffer = 0;
		this->sphere_size = 0;
	}
	void Sphere::draw_sphere() {
		glBindBuffer(GL_ARRAY_BUFFER, this->vBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, this->cBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, this->nBuffer);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, this->sphere_size);
	}
private:
	int vBuffer;
	int cBuffer;
	int nBuffer;
	int sphere_size;
};

#endif 
