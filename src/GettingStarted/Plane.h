#ifndef _PLANE_H
#define _PLANE_H

#include <vector>
#include <sb7.h>
#include <vmath.h>
#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>

struct Plane {
public:
	int vBuffer;
	int tcBuffer;
	int nBuffer;
	int start_index;
	bool has_texture;
	GLint mv_loc;
	GLint shadow_loc;
	GLint light_loc;
	int direction;

	Plane::Plane() {
		this->vBuffer = 0;
		this->tcBuffer = 0;
		this->nBuffer = 0;
		this->has_texture = false;
		this->start_index = 0;
		this->mv_loc = 0;
		this->shadow_loc = 0;
		this->light_loc = 0;
		this->direction = 0;
	}

	Plane::Plane(int vbuf, int ctbuff, int nbuf, bool tex, int start, GLint m_pos, GLint s_pos, GLint l_pos, int n_dir) {
		this->vBuffer = vbuf;
		this->tcBuffer = ctbuff;
		this->nBuffer = nbuf;
		this->has_texture = tex;
		this->start_index = start;
		this->mv_loc = m_pos;
		this->shadow_loc = s_pos;
		this->light_loc = l_pos;
		this->direction = n_dir;
	}

	void Plane::draw(){
		glBindBuffer(GL_ARRAY_BUFFER, this->vBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		
		// Change attribute depending on if it is texture coord or vertex color
		glBindBuffer(GL_ARRAY_BUFFER, this->tcBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);


		glBindBuffer(GL_ARRAY_BUFFER, this->nBuffer);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, this->start_index, 6);
	}
};

#endif //

