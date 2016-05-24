//
// Created by Timofey Tavlintsev on 4/28/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"
#include "Entity.hpp"

using namespace ndk_helper;

#define INERTION_MIN -0.01
#define INERTION_MAX 0.01
#define INERTION_STEP 0.0005
#define BIRD_HEIGHT 0.07
#define BIRD_LENGTH 0.1f
#define BIRD_POS_X -.6f

class Bird: public Entity {

public:

	Bird();
	~Bird() {;;}

	void Init();
	void Unload();
	void onTap();

	void setupGeometry();
	void animateWings(float inertion);
	void randomizeWings();

	void update(float fTime) override;
	void draw(const SHADER_PARAMS &shader_param_, const Mat4 &_view, const Mat4 &_projection) const override;

	void setInertion(float _inert){
		inertion = _inert;
	}
	// return true if possible to decrease
	bool decreaseInertion();

	bool getColliding() { return bColliding; }
	void setColliding(bool is_colliding) { bColliding = is_colliding; }

	Vec3 getPosition() { return world_pos_; }
	Mat4 getModelMatrix() { return Mat4::Translation(world_pos_); }
	CIRCLE & getCircle() { return circle; }

	void reset();
private:

	float inertion;
	bool bInit, bColliding;

	Vec3 world_pos_;

	CIRCLE circle;

	GLuint vbo_;
	GLsync fence;

	std::vector<VERTEX> birdMesh;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec3> uvs;
	MATERIALS materials_;
};


