//
// Created by Timofey Tavlintsev on 4/27/16.
//

#pragma once

#include "NDKHelper.h"
#include "Utils.hpp"
#include "CylinderModel.hpp"

//using namespace cylinderNamespace;

#define CYLINDER_RADIUS 0.04f
#define CYLINDER_HEIGHT 1.f

class CylinderObject {
public:
    CylinderObject()
    : bLoaded(false),
    radius(CYLINDER_RADIUS),
    height(CYLINDER_HEIGHT)
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
    }

	void Unload(){
		if (vbo_) {
			glDeleteBuffers(1, &vbo_);
			vbo_ = 0;
		}

		if (ibo_) {
			glDeleteBuffers(1, &ibo_);
			ibo_ = 0;
		}
		bLoaded = false;
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
	GLuint nbo_;

	bool bLoaded, bBind;

    ndk_helper::Mat4 mat_projection_;
    ndk_helper::Mat4 mat_view_;
    ndk_helper::Mat4 mat_model;

    float radius;
    float height;

    std::vector<uint16_t> indices;
    std::vector<VERTEX> cylVertices;
	std::vector<ndk_helper::Vec3> vertices;
	std::vector<ndk_helper::Vec3> normals;
	std::vector<ndk_helper::Vec2> uvs;
    int32_t stride;

    void generateCylinder() {

	    vertices.clear();
	    normals.clear();
	    indices.clear();
	    uvs.clear();
	    cylVertices.clear();

        stride = sizeof(VERTEX);
//        float x=0, y=0, z=0;
        int radiusSegments = 21;
	    int capSegs = 2;
	    int heightSegments = 2;
//	    float halfH = height*.5f;
	    float heightInc = height/((float)heightSegments-1.f);

	    ndk_helper::Vec3 up_axis = ndk_helper::Vec3(0.f,1.f,0);
	    float angleIncRadius = -1 * (2.f*M_PI/((float)radiusSegments-1.f));
	    float maxTexY   = heightSegments-1.f;
	    if(capSegs > 0) {
		    maxTexY += (capSegs*2)-2.f;
	    }
	    float maxTexYNormalized = (capSegs-1.f) / maxTexY;

	    float newRad;
	    ndk_helper::Vec3 vert;
	    ndk_helper::Vec2 tcoord;
	    ndk_helper::Vec3 normal(0,1,0);

	    std::size_t vertOffset = 0;
		int cntr_vert = 0;

	    // add top cap
	    if (capSegs > 0) {
		    for (int iy = 0; iy < capSegs; iy++) {
			    for (int ix = 0; ix < radiusSegments; ix++) {
				    newRad = valueMap((float) iy, 0, capSegs - 1, 0.0, radius);
				    vert = ndk_helper::Vec3(cos((float) ix * angleIncRadius) * newRad,
											-height,
											sin((float) ix * angleIncRadius) * newRad);
				    vertices.push_back(vert);

				    tcoord = ndk_helper::Vec2((float) ix / ((float) radiusSegments - 1.f),
				                            valueMap(iy, 0, capSegs - 1, 0, maxTexYNormalized));
				    uvs.push_back(tcoord);
				    normals.push_back(normal);

				    cylVertices.push_back(VERTEX());
				    vert.Value(cylVertices[cntr_vert].pos[0], cylVertices[cntr_vert].pos[1], cylVertices[cntr_vert].pos[2]);
				    normal.Value(cylVertices[cntr_vert].normal[0],cylVertices[cntr_vert].normal[1],cylVertices[cntr_vert].normal[3]);
				    cntr_vert++;
			    }
		    }
		    for(int y = 0; y < capSegs-1; y++) {
			    for(int x = 0; x < radiusSegments; x++) {
				    indices.push_back((y)*radiusSegments + x + vertOffset);
				    indices.push_back((y+1)*radiusSegments + x + vertOffset);
			    }
		    }
		    vertOffset = vertices.size();
	    }

			    //maxTexY			 = heightSegments-1.f + capSegs-1.f;
	    float minTexYNormalized = 0;
	    minTexYNormalized = maxTexYNormalized;
	    maxTexYNormalized   = 1.f;
	    maxTexYNormalized = (heightSegments) / maxTexY;

	    // cylinder vertices //
	    for(int iy = 0; iy < heightSegments; iy++) {
		    normal = ndk_helper::Vec3(1,0,0);
		    for(int ix = 0; ix < radiusSegments; ix++) {

			    //newRad = ofMap((float)iy, 0, heightSegments-1, 0.0, radius);
			    vert = ndk_helper::Vec3(cos((float)ix*angleIncRadius) * radius,
			                            heightInc*((float)iy) - height,
			                            sin((float)ix*angleIncRadius) * radius);
			    vertices.push_back(vert);

			    tcoord = ndk_helper::Vec2((float)ix/((float)radiusSegments-1.f),
									    valueMap(iy, 0, heightSegments-1, minTexYNormalized, maxTexYNormalized));
			    uvs.push_back( tcoord );

			    normals.push_back( normal );
			    normal = rotateRad(-angleIncRadius, normal, up_axis);

			    cylVertices.push_back(VERTEX());
			    vert.Value(cylVertices[cntr_vert].pos[0], cylVertices[cntr_vert].pos[1], cylVertices[cntr_vert].pos[2]);
			    normal.Value(cylVertices[cntr_vert].normal[0],cylVertices[cntr_vert].normal[1],cylVertices[cntr_vert].normal[3]);
			    cntr_vert++;
		    }
	    }

	    for(int y = 0; y < heightSegments-1; y++) {
		    for(int x = 0; x < radiusSegments; x++) {
			    indices.push_back( (y)*radiusSegments + x + vertOffset );
			    indices.push_back( (y+1)*radiusSegments + x + vertOffset );
		    }
	    }

	    vertOffset = vertices.size();

	    if (capSegs > 0) {
		    minTexYNormalized = maxTexYNormalized;
		    maxTexYNormalized = 1.f;

		    normal = ndk_helper::Vec3(0, -1, 0);
		    for (int iy = 0; iy < capSegs; iy++) {
			    for (int ix = 0; ix < radiusSegments; ix++) {
				    newRad = valueMap((float) iy, 0, capSegs - 1, radius, 0.0);
				    vert = ndk_helper::Vec3(cos((float) ix * angleIncRadius) * newRad,
				                            0,
				                            sin((float) ix * angleIncRadius) * newRad);
				    vertices.push_back(vert);
				    tcoord = ndk_helper::Vec2((float) ix / ((float) radiusSegments - 1.f),
				                              valueMap(iy, 0, capSegs - 1, minTexYNormalized,
				                                       maxTexYNormalized));
				    uvs.push_back(tcoord);
				    normals.push_back(normal);

				    cylVertices.push_back(VERTEX());
				    vert.Value(cylVertices[cntr_vert].pos[0], cylVertices[cntr_vert].pos[1], cylVertices[cntr_vert].pos[2]);
				    normal.Value(cylVertices[cntr_vert].normal[0],cylVertices[cntr_vert].normal[1],cylVertices[cntr_vert].normal[3]);
				    cntr_vert++;
			    }
		    }

		    for(int y = 0; y < capSegs-1; y++) {
			    for(int x = 0; x < radiusSegments; x++) {
				    indices.push_back((y)*radiusSegments + x + vertOffset );
				    indices.push_back((y+1)*radiusSegments + x + vertOffset);
			    }
		    }
	    }

		num_indices_ = indices.size();
        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

        glBufferDataFromVector(GL_ELEMENT_ARRAY_BUFFER, indices,
                               GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferDataFromVector(GL_ARRAY_BUFFER, cylVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        bLoaded = true;
        //        vertices.clear();
        //        indices.clear();
    }


};
// namespace cylindernamespace

//TRIPPYBIRD_CYLINDEROBJECT_HPP
