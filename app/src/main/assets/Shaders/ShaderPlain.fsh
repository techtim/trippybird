//
// Copyright (C) 2015 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  ShaderPlain.fsh
//

#define USE_PHONG (1)

uniform lowp vec3       vMaterialAmbient;
uniform mediump vec4     vMaterialSpecular;
uniform lowp vec4       vMaterialDiffuse;

uniform lowp int        iObjectType;

varying lowp vec4 colorDiffuse;

#if USE_PHONG
uniform highp vec3   vLight0;
varying mediump vec3 position;
varying mediump vec3 normal;
#else
varying lowp vec4 colorSpecular;
#endif

//vec3 hue2rgb(float hue) {
//    return clamp(
//            abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
//            0.0, 1.0);
//}

void drawCylinder()
{
#if USE_PHONG
    mediump vec3 halfVector = normalize(-vLight0 + position);
    mediump float NdotH = max(dot(normalize(normal), halfVector), 0.0);
    mediump float fPower = vMaterialSpecular.w;
    mediump float specular = pow(NdotH, fPower);

    lowp vec4 colorSpecular = vec4( vMaterialSpecular.xyz * specular, 1 );
    gl_FragColor = colorDiffuse + colorSpecular;
#else
    gl_FragColor = colorDiffuse + colorSpecular;
#endif
}

void drawLine() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

void drawPlane() {
//    lowp vec4 color1 = vec4(hue2rgb(1.f),1);
//    lowp vec4 color2 = vec4(hue2rgb(.5.f),1);
    gl_FragColor = mix(vec4(vMaterialAmbient,1.f),vMaterialDiffuse, position.x);
}

void main(void) {
    if (iObjectType == 0) {
        drawCylinder();
    } else if (iObjectType == 2) {
        drawLine();
    } else if (iObjectType == 3) {
        drawPlane();
    }
}