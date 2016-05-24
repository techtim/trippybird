//
// Created by Timofey Tavlintsev on 5/3/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"
#include "Entity.hpp"

class Plane: public Entity {
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
	void update(float fTime) override{
		;;
	}

	void draw(const SHADER_PARAMS &shader_param_, const Mat4 &_view, const Mat4 &_projection) const override {
		if (!bInit) {
			LOGI("[ERROR] Draw Plane without Init()");
			return;
		}

		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
		                      BUFFER_OFFSET(0));
		glEnableVertexAttribArray(ATTRIB_VERTEX);

		glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(LINE_VERTEX),
		                      BUFFER_OFFSET(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ATTRIB_NORMAL);

		// Update uniforms and draw Planes
		glUniform1i(shader_param_.object_type, TYPE_PLANE);

		// --- Bottom
		Mat4 mat_v = _view * Mat4::Translation(0, 0, 0);
		Mat4 mat_vp = _projection * mat_v;
		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE, mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// --- Top
		mat_v = _view * Mat4::Translation(0, 1, 0);
		mat_vp = _projection * mat_v;
		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE, mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// --- Back
		mat_v = _view * Mat4::RotationX(1.5708) * Mat4::Translation(0, 2,0.f);
//		mat_v = mat_v * Mat4::Translation(0, 2,0.f);
		mat_vp = _projection * mat_v;
		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE, mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	GLuint vbo_;
	bool bInit;

};
