//
// Created by Timofey Tavlintsev on 4/29/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"
#include "CylinderObject.hpp"

#define OBSTACLES_MIN_HEIGHT .2f
#define OBSTACLES_DISTANCE 0.3f

class Obstacle {
public:
	Obstacle() {
		pos = ndk_helper::Vec3(2.f, 0, 0);
		randomizePos(pos, OBSTACLES_MIN_HEIGHT, 1.f-OBSTACLES_MIN_HEIGHT*2);
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}
	Obstacle(const ndk_helper::Vec3 & _pos) {
		pos = _pos;
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}

	Obstacle(const float & x_pos) {
		pos = ndk_helper::Vec3(x_pos, 0, 0);
		randomizePos(pos, OBSTACLES_MIN_HEIGHT, 1.f-OBSTACLES_MIN_HEIGHT-OBSTACLES_DISTANCE);
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}

	void Update(float fTime){
		if (last_time_==0) last_time_ = fTime;
		pos-=  ndk_helper::Vec3(0.001, 0 ,0 );
		last_time_ = fTime;
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}

	Obstacle createPair() {
		return Obstacle(pos+ndk_helper::Vec3(0,OBSTACLES_DISTANCE+CYLINDER_HEIGHT,0));
	}

	void randomizePos(ndk_helper::Vec3 & _pos, float from, float range){
		float x,y,z;
		_pos.Value(x,y,z);
		_pos = ndk_helper::Vec3(x, from+range/10*(rand()%10), z);
	}

	const ndk_helper::Mat4 & getModelMatrix() { return mat_model; }
	ndk_helper::Vec3 & getPosition() { return pos; }
	float getPositionX() { return rect.x+CYLINDER_RADIUS; }
	float getPositionY() { return rect.y; }
	const RECT_WH & getRect() { return rect; }

private:
	ndk_helper::Mat4 mat_model;
	ndk_helper::Vec3 pos;
	RECT_WH rect;
	float last_time_;
	void updateRectWithPos(ndk_helper::Vec3 & _pos) {
		_pos.Value(rect.x, rect.y, rect.z);
		rect.x-=CYLINDER_RADIUS;
		rect.width = CYLINDER_RADIUS*2;
		rect.height = rect.y>1.f ? rect.y-1.f: rect.y;
	}
};