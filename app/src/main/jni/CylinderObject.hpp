//
// Created by Timofey Tavlintsev on 4/27/16.
//

#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "vecmath.h"

#include "Utils.hpp"
#include "CylinderModel.hpp"

//using namespace cylinderNamespace;

#define CYLINDER_RADIUS 0.02

class CylinderObject {
public:
    CylinderObject()
    : bLoaded(false),
    radius(CYLINDER_RADIUS),
    height(1.f)
    {;;}
    ~CylinderObject(){
        if (vbo_) {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }

        if (ibo_) {
            glDeleteBuffers(1, &ibo_);
            ibo_ = 0;
        }
    }

    void Init() {
            generateCylinder();
            bLoaded = true;
            return;
//             Create Index buffer
        num_indices_ = sizeof(cylinderIndices) / sizeof(cylinderIndices[0]);
        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinderIndices), cylinderIndices,
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Create VBO
        num_vertices_ = sizeof(cylinderVertices) / sizeof(cylinderVertices[0]) / 3;
        int32_t stride = sizeof(CYLINDER_VERTEX);
        int32_t index = 0;
        CYLINDER_VERTEX* p = new CYLINDER_VERTEX[num_vertices_];
        for (int32_t i = 0; i < num_vertices_; ++i) {
            p[i].pos[0] = cylinderVertices[index];
            p[i].pos[1] = cylinderVertices[index + 1];
            p[i].pos[2] = cylinderVertices[index + 2];

            p[i].normal[0] = cylinderNormals[index];
            p[i].normal[1] = cylinderNormals[index + 1];
            p[i].normal[2] = cylinderNormals[index + 2];
            index += 3;
        }
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, stride * num_vertices_, p, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] p;
        bLoaded = true;
    }

    void bind() {
        if (!bLoaded || bBind) return;

        // Bind the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        // Pass the vertex data
        glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, stride,
                              BUFFER_OFFSET(0));
        glEnableVertexAttribArray(ATTRIB_VERTEX);

        glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, stride,
                              BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIB_NORMAL);

//        glDrawArrays(GL_TRIANGLES, 0, num_vertices_);
//        GL_TRIANGLE_STRIP

//         Bind the IB
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        bBind = true;
    }

    void unbind() {
        if (!bBind) return;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        bBind = false;
    }
    void draw() {
        if (!bBind) return;
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    }

    float getRadius() { return radius; }
    float getHeight() { return height; }
private:
    int32_t num_indices_;
    int32_t num_vertices_;
    GLuint ibo_;
    GLuint vbo_;

    bool bLoaded, bBind;

    ndk_helper::Mat4 mat_projection_;
    ndk_helper::Mat4 mat_view_;
    ndk_helper::Mat4 mat_model;

    float radius;
    float height;

    struct CYLINDER_VERTEX {
        float pos[3];
        float normal[3];
    };

    std::vector<uint16_t> indices;
    std::vector<CYLINDER_VERTEX> vertices;

    int32_t stride;

    void generateCylinder() {
        stride = sizeof(CYLINDER_VERTEX);
        float x=0, y=0, z=0;
        int sides = 20;
        num_vertices_ = sides * 2;
        const float theta = 2. * M_PI / (float)sides;
        float c = cosf(theta);
        float s = sinf(theta);
        // coordinates on top of the circle, on xz plane
        float x2 = radius, z2 = 0;
        //        CYLINDER_VERTEX* p = new CYLINDER_VERTEX[num_vertices_];
        // make the strip

        vertices.resize(num_vertices_);

        int vertNum = 0;

//        vertices[vertNum].pos[0] = 0;
//        vertices[vertNum].pos[1] = y;
//        vertices[vertNum].pos[2] = 0;
//
//        vertices[vertNum].normal[0] = 0;
//        vertices[vertNum].normal[1] = 1;
//        vertices[vertNum].normal[2] = 0;
//
//        vertNum++;
        vertices[vertNum].pos[0] = 0;
        vertices[vertNum].pos[1] = y+height;
        vertices[vertNum].pos[2] = 0;

        vertices[vertNum].normal[0] = 0;
        vertices[vertNum].normal[1] = 1;
        vertices[vertNum].normal[2] = 0;

        vertNum++;
        for(int i=0; i<=sides; i++) {
            // texture coord
//            const float tx = (float)i/sides;
            // normal
            const float nf = 1./sqrt(x2*x2+z2*z2),
                    xn = x2*nf, zn = z2*nf;

            vertices[vertNum].pos[0] = x+x2;
            vertices[vertNum].pos[1] = y;
            vertices[vertNum].pos[2] = z+z2;

            vertices[vertNum+1].pos[0] = x+x2;
            vertices[vertNum+1].pos[1] = y+height;
            vertices[vertNum+1].pos[2] = z+z2;

            vertices[vertNum].normal[0] = vertices[vertNum+1].normal[0] = xn;
            vertices[vertNum].normal[1] = vertices[vertNum+1].normal[1] = 0;
            vertices[vertNum].normal[2] = vertices[vertNum+1].normal[2] = zn;


            if (vertNum>2) {
                indices.push_back(vertNum);
                indices.push_back(vertNum - 2);
                indices.push_back(vertNum + 1);
                indices.push_back(vertNum - 1);
                indices.push_back(0);
                indices.push_back(vertNum + 1);
                indices.push_back(vertNum + 1);
                indices.push_back(vertNum + 2);
            }
            vertNum+=2;

//            indices.push_back(1);
//                indices.push_back(0);
//                indices.push_back(i);
//            } else if ( i>0 && i%3==0) {
//                indices.push_back(i);
//                indices.push_back(i-1);
//                indices.push_back(i+1);
//            }
            //            glNormal3f(xn,0,zn);
            //            glTexCoord2f(tx,0);
            //            glVertex3f(x+x2,y,z+z2);
            //            glNormal3f(xn,0,zn);
            //            glTexCoord2f(tx,1);
            //            glVertex3f(x+x2,y+height,z+z2);
            // next position
            const float x3 = x2;
            x2 = c * x2 - s * z2;
            z2 = s * x3 + c * z2;
        }
        indices[indices.size()-1] = 0;
        num_indices_ = indices.size();
        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

        glBufferDataFromVector(GL_ELEMENT_ARRAY_BUFFER, indices,
                               GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferDataFromVector(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        bLoaded = true;
        //        vertices.clear();
        //        indices.clear();
    }


};
// namespace cylindernamespace

//TRIPPYBIRD_CYLINDEROBJECT_HPP
