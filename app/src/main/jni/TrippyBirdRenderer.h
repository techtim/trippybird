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
#include <jni.h>
#include <errno.h>

#include <vector>
#include <math.h>
#include <stdlib.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>

#define CLASS_NAME "android/app/NativeActivity"
#define APPLICATION_CLASS_NAME "com/sample/teapot/TeapotApplication"

#include "NDKHelper.h"

#include "Utils.hpp"
#include "CylinderObject.hpp"
#include "Bird.hpp"
#include "Obstacle.hpp"

class TrippyBirdRenderer {
 public:
    TrippyBirdRenderer();
    virtual ~TrippyBirdRenderer();
    void Init();
    void Render();
    void Update(float dTime);
    void onTap();
	void onDoubleTap();
    bool Bind(ndk_helper::TapCamera* camera);
    void Unload();
    void UpdateViewport();


  private:

    SHADER_PARAMS shader_param_;
    bool LoadShaders(SHADER_PARAMS* params, const char* strVsh,
                     const char* strFsh);

    ndk_helper::Mat4 mat_projection_;
    ndk_helper::Mat4 mat_view_;
    ndk_helper::Mat4 mat_model_;

    ndk_helper::TapCamera* camera_;
	bool bCameraActive;

    CylinderObject  cylinderObj_;
    Bird            bird_;

    std::vector<Obstacle> obstacles_;
    float obstacles_dist;
	std::vector<Obstacle> & getObstacles() { return obstacles_; }

};

#endif
