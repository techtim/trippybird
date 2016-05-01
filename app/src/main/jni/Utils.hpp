//
// Created by Timofey Tavlintsev on 4/27/16.
//

#pragma once

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <vector>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

enum SHADER_ATTRIBUTES {
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    ATTRIB_UV,
};

struct SHADER_PARAMS {
    GLuint program_;
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;

    GLuint matrix_projection_;
    GLuint matrix_view_;
};

struct CYLINDER_MATERIALS {
    float diffuse_color[3];
    float specular_color[4];
    float ambient_color[3];
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

struct PackedVertex{
    ndk_helper::Vec3 position;
    ndk_helper::Vec2 uv;
    ndk_helper::Vec3 normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    };
};

bool getSimilarVertexIndex_fast(
        PackedVertex & packed,
        std::map<PackedVertex,unsigned short> & VertexToOutIndex,
        unsigned short & result
){
    std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() ){
        return false;
    }else{
        result = it->second;
        return true;
    }
}

static void indexVBO(
        std::vector<ndk_helper::Vec3> & in_vertices,
        std::vector<ndk_helper::Vec2> & in_uvs,
        std::vector<ndk_helper::Vec3> & in_normals,

        std::vector<unsigned short> & out_indices,
        std::vector<ndk_helper::Vec3> & out_vertices,
        std::vector<ndk_helper::Vec2> & out_uvs,
        std::vector<ndk_helper::Vec3> & out_normals
){
    std::map<PackedVertex,unsigned short> VertexToOutIndex;

    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){

        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};


        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( in_vertices[i]);
            out_uvs     .push_back( in_uvs[i]);
            out_normals .push_back( in_normals[i]);
            unsigned short newindex = (unsigned short)out_vertices.size() - 1;
            out_indices .push_back( newindex );
            VertexToOutIndex[ packed ] = newindex;
        }
    }
}