//
// Created by Timofey Tavlintsev on 4/28/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"

#define INERTION_MIN -0.01
#define INERTION_MAX 0.01
#define INERTION_STEP 0.0005

class Bird {

public:
//    static FlyingUpState flyingUp;
//    static FlyingDownState flyingDown;
	enum State {
		FLYING_DOWN,
		FLYING_UP
	};
	Bird();
	~Bird() {;;}

	void Init();
	void Unload();
	void onTap();
	void update();

	void draw();

	void setInertion(float _inert){
		inertion = _inert;
	}
	// return true if possible to decrease
	bool decreaseInertion();

	bool getColliding() { return bColliding; }
	void setColliding(bool is_colliding) { bColliding = is_colliding; }

	ndk_helper::Vec3 getPosition() { return world_pos; }
	const ndk_helper::Mat4 & getModelMatrix() { return mat_model; }
	void setState(State  new_state) { state_ = new_state; }

	CIRCLE & getCircle() { return circle; }
private:
	State state_;
//    BirdState * state_;
	ndk_helper::Vec3 world_pos;
	float inertion;
	CIRCLE circle;

	GLuint ibo_;
	GLuint vbo_left, vbo_right;

	bool bColliding;
	std::vector<VERTEX> wingLeft;
	std::vector<VERTEX> wingRight;
	ndk_helper::Mat4 mat_model;
};


