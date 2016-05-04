//
// Created by Timofey Tavlintsev on 5/3/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"

class Plane {
public:
	Plane() {
		bInit =false;
	}

	~Plane() {
		glDeleteBuffers(1,&vbo_);
	}

	void Init() {
		VERTEX vertices[] = {
				{{2,0,2},   {0.f,1.f,0.f}},
				{{-2,0,2},  {0.f,1.f,0.f}},
				{{2,0,-2},  {0.f,1.f,0.f}},
				{{-2,0,-2}, {0.f,1.f,0.f}},
		};
		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(VERTEX), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		bInit = true;
	}

	void Unload() {
		if (vbo_) {
			glDeleteBuffers(1, &vbo_);
			vbo_ = 0;
		}
	}

	void draw() {
		if (!bInit) {
			LOGI("[ERROR] Draw Plane without Init()");
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
		                      BUFFER_OFFSET(0));
		glEnableVertexAttribArray(ATTRIB_VERTEX);

		glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(LINE_VERTEX),
		                      BUFFER_OFFSET(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ATTRIB_NORMAL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	GLuint vbo_;
	bool bInit;


};
