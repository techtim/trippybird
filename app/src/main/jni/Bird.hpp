//
// Created by Timofey Tavlintsev on 4/28/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"

#define INERTION_MIN -0.01
#define INERTION_MAX 0.01
#define INERTION_STEP 0.0005
#define BIRD_HEIGHT 0.07
#define BIRD_LENGTH 0.1f

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

	void animateWings(float inertion);
	void draw();

	void setInertion(float _inert){
		inertion = _inert;
	}
	// return true if possible to decrease
	bool decreaseInertion();

	bool getColliding() { return bColliding; }
	void setColliding(bool is_colliding) { bColliding = is_colliding; }

	ndk_helper::Vec3 getPosition() { return world_pos; }
	ndk_helper::Mat4 getModelMatrix() { return ndk_helper::Mat4::Translation(world_pos); }
	void setState(State  new_state) { state_ = new_state; }

	CIRCLE & getCircle() { return circle; }
private:
	State state_;
	float inertion;
	bool bInit, bColliding;

	ndk_helper::Vec3 world_pos;

	CIRCLE circle;

	GLuint ibo_;
	GLuint vbo_, vbo_right;
	GLsync fence;


	std::vector<VERTEX> birdMesh;
	std::vector<VERTEX> wingRight;
	std::vector<ndk_helper::Vec3> vertices;
	std::vector<ndk_helper::Vec3> normals;
	std::vector<ndk_helper::Vec3> uvs;
	ndk_helper::Mat4 mat_model;
};


