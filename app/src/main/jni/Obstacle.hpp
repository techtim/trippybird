//
// Created by Timofey Tavlintsev on 4/29/16.
//

#pragma once

#include "NDKHelper.h"
#include <memory>
#include "Utils.hpp"
#include "Entity.hpp"
#include "CylinderObject.hpp"

using namespace ndk_helper;

#define OBSTACLES_MIN_HEIGHT .2f
#define OBSTACLES_UP_DISTANCE 0.3f
#define OBSTACLES_SPEED 0.003

class Obstacle: public Entity {
public:
	Obstacle() {
		world_pos_ = Vec3(2.f, 0, 0);
		randomizePos(world_pos_, OBSTACLES_MIN_HEIGHT, 1.f - OBSTACLES_MIN_HEIGHT * 2);
		updateRectWithPos(world_pos_);
	}
	Obstacle(const Vec3 & _pos) {
		world_pos_ = _pos;
		updateRectWithPos(world_pos_);
	}

	Obstacle(const float & x_pos) {
		world_pos_ = Vec3(x_pos, 0, 0);
		randomizePos(world_pos_, OBSTACLES_MIN_HEIGHT, 1.f - OBSTACLES_MIN_HEIGHT -
		                                               OBSTACLES_UP_DISTANCE);
		updateRectWithPos(world_pos_);
	}

	void update(float fTime) override {
		if (last_time_==0) last_time_ = fTime;
		world_pos_ -= Vec3(fTime, 0 ,0 );
		last_time_ = fTime;
		updateRectWithPos(world_pos_);
	}

	void updateRectWithPos(Vec3 & _pos) {
		_pos.Value(rect.x, rect.y, rect.z);
		rect.x -= CYLINDER_RADIUS;
		rect.y -= CYLINDER_HEIGHT;
		rect.width = CYLINDER_RADIUS*2;
		rect.height = CYLINDER_HEIGHT;
	}

	void draw(const SHADER_PARAMS &shader_param_, const Mat4 &_view, const Mat4 &_projection) const override{
		if (drawableObject_ == nullptr) {
			LOGI("Draw Obstacle with undefined Cylynder Object");
			return;
		}
		// --- position uniforms
		Mat4 mat_v = _view * Mat4::Translation(world_pos_);
		Mat4 mat_vp = _projection * mat_v;
		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE, mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());

		// --- material uniforms
		glUniform4f(shader_param_.material_diffuse_, materials_.diffuse_color[0],
		            materials_.diffuse_color[1], materials_.diffuse_color[2], 1.f);

		glUniform4f(shader_param_.material_specular_, materials_.specular_color[0],
		            materials_.specular_color[1], materials_.specular_color[2],
		            materials_.specular_color[3]);

		glUniform3f(shader_param_.material_ambient_, materials_.ambient_color[0],
		            materials_.ambient_color[1], materials_.ambient_color[2]);

		// --- Set type of object for shader
		glUniform1i(shader_param_.object_type, TYPE_CYLINDER);

		// --- Draw geometry
		drawableObject_->bind();
		drawableObject_->draw();
		drawableObject_->unbind();
	}

	// --- Set pointer to cylider geometry buffers
	void setDrawableObject(std::shared_ptr<CylinderObject> obj) {
		drawableObject_ = obj;
	}

	Obstacle createPair() {
		Obstacle obs(world_pos_ + Vec3(0, OBSTACLES_UP_DISTANCE + CYLINDER_HEIGHT, 0));
		obs.setDrawableObject(drawableObject_);
		obs.setMaterials(getMaterials());
		return obs;
	}

	void randomizePos(Vec3 & _pos, float from, float range){
		float x,y,z;
		_pos.Value(x,y,z);
		_pos = Vec3(x, from+range/10*(rand()%10), z);
	}

	const Mat4 getModelMatrix() { return Mat4::Translation(world_pos_); }

	Vec3 & getPosition() { return world_pos_; }
	float getPositionX() { return rect.x; }
	float getPositionY() { return rect.y; }
	const RECT_WH & getRect() { return rect; }

private:
	RECT_WH rect;
	float last_time_;
	std::shared_ptr<CylinderObject> drawableObject_;

};