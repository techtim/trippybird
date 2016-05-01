//
// Created by Timofey Tavlintsev on 4/29/16.
//

#pragma once

#include <stdlib.h>
#include "NDKHelper.h"
#include "Utils.hpp"
#include "CylinderObject.hpp"

class Obstacle {
public:
	Obstacle() {
		pos = ndk_helper::Vec3(2.f, 0, 0);
		randomize(pos);
		updateRectWithPos(pos);
	}
	Obstacle(const float & x_pos) {
		pos = ndk_helper::Vec3(x_pos, 0, 0);
		randomize(pos);
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}

	void randomize(ndk_helper::Vec3 & _pos){
		float x,y,z;
		_pos.Value(x,y,z);
		_pos = ndk_helper::Vec3(x, 1.f-.04*(rand()%20), z);
	}

	void Update(float fTime){
		pos-=  ndk_helper::Vec3(0.001, 0 ,0 );
		updateRectWithPos(pos);
		mat_model = ndk_helper::Mat4::Translation(pos);
	}

	const ndk_helper::Mat4 & getModelMatrix() { return mat_model; }
	const ndk_helper::Vec3 & getPosition() { return pos; }
	float getPositionX() { return rect.x+CYLINDER_RADIUS; }
	const RECT_WH & getRect() { return rect; }

private:
	ndk_helper::Mat4 mat_model;
	ndk_helper::Vec3 pos;
	RECT_WH rect;

	void updateRectWithPos(ndk_helper::Vec3 & _pos) {
		_pos.Value(rect.x, rect.y, rect.z);
		rect.x-=CYLINDER_RADIUS;
		rect.width = CYLINDER_RADIUS*2;
		rect.height = rect.y;
	}
};