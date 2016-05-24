//
// Created by Timofey Tavlintsev on 5/22/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"

using namespace ndk_helper;

class Entity {
public:
	virtual void update(float fTime)=0;
	virtual void draw(const SHADER_PARAMS &shader_params_, const Mat4 &_view, const Mat4 &_projection) const=0;
	void setMaterials(const MATERIALS & _materials) {
		materials_ = _materials;
	}
	const MATERIALS & getMaterials() {
		return materials_;
	}

protected:
	Vec3 world_pos_;
	MATERIALS materials_;
	double elapsedTime, currentTime;
};
