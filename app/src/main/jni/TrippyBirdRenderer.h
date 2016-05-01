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

struct TEAPOT_VERTEX {
  float pos[3];
  float normal[3];
};

struct TEAPOT_MATERIALS {
  float diffuse_color[3];
  float specular_color[4];
  float ambient_color[3];
};

class TrippyBirdRenderer {
 public:
    TrippyBirdRenderer();
    virtual ~TrippyBirdRenderer();
    void Init();
    void Render();
    void Update(float dTime);
    void onTap() { bird_.onTap(); }
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

    CylinderObject  cylinderObj_;
    Bird            bird_;

    struct CYLINDER_VERTEX {
        float pos[3];
        float normal[3];
    };

    std::vector<uint16_t> indices;
    std::vector<CYLINDER_VERTEX> vertices;
    std::vector<Obstacle> obstacles_;
    float obstacles_dist;
    int32_t stride;
    bool bLoaded;

    template <class T>
    inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
        glBufferData(target, v.size() * sizeof(T), &v[0], usage);
    }
};

#endif
