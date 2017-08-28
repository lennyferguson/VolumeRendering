#ifndef _FLOOR_H_
#define _FLOOR_H

#include <sb7.h>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>
#include <vector>

struct Floor {
public:
	Floor::Floor(int vBufferIndex, int texBufferIndex) {
		vIndex = vBufferIndex;
		tIndex = texBufferIndex;
	}

	Floor::Floor() {
		vIndex = 0;
		tIndex = 0;
	}
	void Floor::draw_floor() {
		glBindBuffer(GL_ARRAY_BUFFER, this->vIndex);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, this->tIndex);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
private:
	int vIndex;
	int tIndex;
};


#endif