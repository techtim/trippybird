//
// Created by Timofey Tavlintsev on 4/27/16.
//

#pragma once

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <vector>
#include <stdlib.h>

#define FLT_EPSILON 1.19209290E-07F
#define BUFFER_OFFSET(i) ((char*)NULL + (i))


enum SHADER_ATTRIBUTES {
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    ATTRIB_UV,
};

enum OBJECT_TYPES {
	TYPE_CYLINDER,
	TYPE_BIRD,
	TYPE_LINE,
	TYPE_PLANE
};

struct SHADER_PARAMS {
    GLuint program_;
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;

    GLuint matrix_projection_;
    GLuint matrix_view_;
	GLuint object_type;
};

struct CYLINDER_MATERIALS {
    float diffuse_color[3];
    float specular_color[4];
    float ambient_color[3];
};

struct VERTEX {
	float pos[3];
	float normal[3];
//	float uv[2];
};

struct CIRCLE {
    float x,y,z;
    float r;
};

struct RECT_WH
{
    float x,y,z;
    float width;
    float height;
};

struct OBSTACLE {
    ndk_helper::Mat4 model_view;
    ndk_helper::Vec3 pos;
    RECT_WH rect;
};

struct SAVED_STATE {
	std::vector<OBSTACLE> obstacles;
	ndk_helper::Vec3 birdPos;
};

//bool intersects(CIRCLE circle, RECT_WH rect)
//{
//    float dist_x = fabsf(circle.x - rect.x);
//    float dist_y = fabsf(circle.y - rect.y);
//
//    if (dist_x > (rect.width/2 + circle.r)) { return false; }
//    if (dist_y > (rect.height/2 + circle.r)) { return false; }
//
//    if (dist_x <= (rect.width/2)) { return true; }
//    if (dist_y <= (rect.height/2)) { return true; }
//
//    double cornerDistance_sq = pow(dist_x - rect.width/2, 2) + pow(dist_y - rect.height/2, 2);
//
//    return (cornerDistance_sq <= pow(circle.r, 2));
//}

//static bool valueInRange(int value, int min, int max)
//{ return (value >= min) && (value <= max); }

//static bool rectOverlap(const RECT_WH &A, const RECT_WH &B) {
//    bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) ||
//                    valueInRange(B.x, A.x, A.x + A.width);
//
//    bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) ||
//                    valueInRange(B.y, A.y, A.y + A.height);
//
//    return xOverlap && yOverlap;
//}

template <class T>
inline void glBufferDataFromVector(GLenum target, const std::vector<T>& v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), &v[0], usage);
}


inline float valueMap(float value, float inputMin, float inputMax, float outputMin, float outputMax) { // , bool clamp = false

	if (fabs(inputMin - inputMax) < FLT_EPSILON){
		return outputMin;
	} else {
		float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

//        if( clamp ){
//            if(outputMax < outputMin){
//                if( outVal < outputMax )outVal = outputMax;
//                else if( outVal > outputMin )outVal = outputMin;
//            }else{
//                if( outVal > outputMax )outVal = outputMax;
//                else if( outVal < outputMin )outVal = outputMin;
//            }
//        }
		return outVal;
	}

}

inline ndk_helper::Vec3 rotateRad(float angle, const ndk_helper::Vec3& vec,  const ndk_helper::Vec3& axis ) {
    ndk_helper::Vec3 v = vec;
    float x, y, z;
    v.Value(x,y,z);

    ndk_helper::Vec3 ax = axis;
    ax.Normalize();
    float ax_x,ax_y,ax_z;
    ax.Value(ax_x,ax_y,ax_z);

    float sina = sin( angle );
    float cosa = cos( angle );
    float cosb = 1.0f - cosa;

    float nx=  x*(ax_x*ax_x*cosb + cosa)
            + y*(ax_x*ax_y*cosb - ax_z*sina)
            + z*(ax_x*ax_z*cosb + ax_y*sina);
    float ny = x*(ax_y*ax_x*cosb + ax_z*sina)
            + y*(ax_y*ax_y*cosb + cosa)
            + z*(ax_y*ax_z*cosb - ax_x*sina);
    float nz = x*(ax_z*ax_x*cosb - ax_y*sina)
            + y*(ax_z*ax_y*cosb + ax_x*sina)
            + z*(ax_z*ax_z*cosb + cosa);

    return ndk_helper::Vec3(nx,ny,nz);
}

struct LINE_VERTEX {
	float pos[3];
	float color[4];
};

inline void drawAxis( float size ) {
//	std::vector<LINE_VERTEX> vertices;
//	vertices.push_back()
	GLuint vbo_;
	LINE_VERTEX vertices[] = {
			{{0,0,0}, {1.f,0,0,1.f}},
			{{size,0,0}, {1,0,0,1.f}},
			{{0,0,0}, {0,1.f,0,1.f}},
			{{0,size,0}, {0,1.f,0,1.f}},
			{{0,0,0}, {0,0,1.f,1.f}},
			{{0,0,size}, {0,0,1.f,1.f}}
	};

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(LINE_VERTEX), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(LINE_VERTEX),
	                      BUFFER_OFFSET(0));
	glEnableVertexAttribArray(ATTRIB_VERTEX);

//	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(LINE_VERTEX),
//	                      BUFFER_OFFSET(3 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glDrawArrays(GL_LINES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_);

}

struct PackedVertex{
    ndk_helper::Vec3 position;
    ndk_helper::Vec2 uv;
    ndk_helper::Vec3 normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    };
};

//bool getSimilarVertexIndex_fast(
//        PackedVertex & packed,
//        std::map<PackedVertex,unsigned short> & VertexToOutIndex,
//        unsigned short & result
//){
//    std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
//    if ( it == VertexToOutIndex.end() ){
//        return false;
//    }else{
//        result = it->second;
//        return true;
//    }
//}
//
//void indexVBO(
//        std::vector<ndk_helper::Vec3> & in_vertices,
//        std::vector<ndk_helper::Vec2> & in_uvs,
//        std::vector<ndk_helper::Vec3> & in_normals,
//
//        std::vector<uint16_t> & out_indices,
//        std::vector<ndk_helper::Vec3> & out_vertices,
//        std::vector<ndk_helper::Vec2> & out_uvs,
//        std::vector<ndk_helper::Vec3> & out_normals
//){
//    std::map<PackedVertex,unsigned short> VertexToOutIndex;
//
//    // For each input vertex
//    for ( unsigned int i=0; i<in_vertices.size(); i++ ){
//
//        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
//
//
//        // Try to find a similar vertex in out_XXXX
//        uint16_t index;
//        bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);
//
//        if ( found ){ // A similar vertex is already in the VBO, use it instead !
//            out_indices.push_back( index );
//        }else{ // If not, it needs to be added in the output data.
//            out_vertices.push_back( in_vertices[i]);
//            out_uvs     .push_back( in_uvs[i]);
//            out_normals .push_back( in_normals[i]);
//            uint16_t newindex = (uint16_t)out_vertices.size() - 1;
//            out_indices .push_back( newindex );
//            VertexToOutIndex[ packed ] = newindex;
//        }
//    }
//}