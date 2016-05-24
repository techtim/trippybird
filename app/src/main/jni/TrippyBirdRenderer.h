/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//--------------------------------------------------------------------------------
// Teapot Renderer.h
// Renderer for teapots
//--------------------------------------------------------------------------------
#ifndef _TEAPOTRENDERER_H
#define _TEAPOTRENDERER_H

//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "NDKHelper.h"
#include <memory>

#define CLASS_NAME "android/app/NativeActivity"
#define SEC_PER_UPDATE 0.016
#define SPACE_BETWEEN_OBSTACLES 0.5

#include "Utils.hpp"
#include "CylinderObject.hpp"
#include "Bird.hpp"
#include "Obstacle.hpp"
#include "Plane.hpp"

using namespace ndk_helper;

class TrippyBirdRenderer {
 public:
    TrippyBirdRenderer();
    virtual ~TrippyBirdRenderer();
    void Init();
    void Render();
    void Update(double dTime);
	void UpdateViewport();
	void UpdateCylinderMaterial(MATERIALS& mat, float hue_pos);

    void onTap();
	void onDoubleTap();
    bool Bind(TapCamera* camera);
    void Unload();

	void toogleCamera() { bCameraActive = !bCameraActive; camera_->Reset(true);}
	void setPause(bool isPaused);
	void setupWorld();
private:

    SHADER_PARAMS shader_param_;
    bool LoadShaders(SHADER_PARAMS* params, const char* strVsh,
                     const char* strFsh);

    Mat4 mat_projection_;
    Mat4 mat_view_;
    Mat4 mat_model_;

    TapCamera* camera_;
	bool bCameraActive, bGamePaused, bVerticalView;

	std::shared_ptr<CylinderObject> cylinderObj_;

	MATERIALS materialCyl;
    Bird            bird_;
	Plane plane_;
	float gradientPos;
	RECT_WH topPlaneRect, bottomPlaneRect;

    std::vector<Obstacle> obstacles_;
    float obstacles_dist;
	float obstacles_speed;
	size_t num_obstacles;
	double currentTime, elapsedTime, lagTime;

	const float CAM_NEAR = .6f;
	const float CAM_FAR = 5.f;
	const float CAM_X = 0.f;
	const float CAM_Y = 0.5f;
	const float CAM_Z = 1.f;
	const float CAM_VERT_X = -1.5f;
	const float CAM_VERT_Y = 0.5f;
	const float CAM_VERT_Z = 0.5f;
	const float LIGHT_X = -3.f;
	const float LIGHT_Y = .5f;
	const float LIGHT_Z = 5.f;
};

#endif
