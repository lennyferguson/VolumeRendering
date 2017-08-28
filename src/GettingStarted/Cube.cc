#include <sb7.h>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>
#include <vector>

struct Cube {
public:
	Cube::Cube(int vbuffer_index, int cbuffer_index, int nbuffer_index) {
		this->vBufferIndex = vbuffer_index;
		this->cBufferIndex = cbuffer_index;
		this->nBufferIndex = nbuffer_index;
	}
	void Cube::draw_cube(GLint mv_loc, GLuint ** buffers, vmath::mat4& mv_mat) {
		glUniformMatrix4fv(mv_loc, 1, false, mv_mat);

		glBindBuffer(GL_ARRAY_BUFFER, (*buffers)[this->vBufferIndex]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, (*buffers)[this->cBufferIndex]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, (*buffers)[this->nBufferIndex]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
private:
	int vBufferIndex;
	int cBufferIndex;
	int nBufferIndex;
};