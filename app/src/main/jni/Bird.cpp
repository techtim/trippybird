//
// Created by Timofey Tavlintsev on 4/28/16.
//


#include "Bird.hpp"
//FlyingUpState Bird::flyingUp;
//FlyingDownState Bird::flyingDown;
Bird::Bird():
		state_(FLYING_DOWN),
		inertion(0.f),
		bInit(false),
		bColliding(false)
{
	reset();
	circle.r=BIRD_HEIGHT/2;
	world_pos.Value(circle.x, circle.y, circle.z);
	circle.x -= BIRD_HEIGHT/2;
	circle.y += BIRD_HEIGHT/2;
}

void Bird::Init() {

	birdMesh.resize(6);

	ndk_helper::Vec3 u,v,normal;
	vertices.clear();
	normals.clear();
	vertices.push_back(ndk_helper::Vec3(0, 0, 0));
	vertices.push_back(ndk_helper::Vec3(-BIRD_LENGTH, BIRD_HEIGHT, BIRD_HEIGHT));
	vertices.push_back(ndk_helper::Vec3(-BIRD_LENGTH, 0, 0));
//	u = vertices[1]-vertices[0];
//	v = vertices[2]-vertices[0];
//	normal = u.Cross(v);
	normal = ndk_helper::Vec3(0, 0, -1.f);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	vertices.push_back(ndk_helper::Vec3(0, 0, 0));
	vertices.push_back(ndk_helper::Vec3(-BIRD_LENGTH, BIRD_HEIGHT, -BIRD_HEIGHT));
	vertices.push_back(ndk_helper::Vec3(-BIRD_LENGTH, 0, 0));
//	u = vertices[4]-vertices[3];
//	v = vertices[5]-vertices[3];
//	normal = u.Cross(v);
	normal = ndk_helper::Vec3(-1, 0, -1.f);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

	birdMesh.reserve(vertices.size());
	for (uint8_t i=0; i<vertices.size();i++) {
		vertices[i].Value(birdMesh[i].pos[0], birdMesh[i].pos[1],birdMesh[i].pos[2]);
		normals[i].Value(birdMesh[i].normal[0], birdMesh[i].normal[1],birdMesh[i].normal[2]);
	}

	bInit = true;
}

void Bird::Unload() {
	if (vbo_) {
		glDeleteBuffers(1, &vbo_);
		vbo_ = 0;
	}
}

void Bird::update() {
	if ((circle.y >0 && circle.y < 1.f) || inertion>0) {
		world_pos += ndk_helper::Vec3(0, inertion, 0);
	} else {
		bColliding = true;
	}
	animateWings(inertion);
	world_pos.Value(circle.x, circle.y, circle.z);
	mat_model = ndk_helper::Mat4::Translation(circle.x, circle.y, circle.z);
	circle.x -= BIRD_HEIGHT/2;
	circle.y += BIRD_HEIGHT/2;
	decreaseInertion();
}

void Bird::reset() {
	world_pos = ndk_helper::Vec3(-0.6, 0.5, 0);
	bColliding = false;
}

void Bird::animateWings(float _inert) {
	birdMesh[1].pos[1] = BIRD_HEIGHT-_inert*4.f;
	birdMesh[4].pos[1] = BIRD_HEIGHT-_inert*4.f;
}

void Bird::draw() {
	if (!bInit) {
		LOGI("[ERROR] Draw Bird without Init()");
		return;
	}
	// Bind and map buffer.

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	glBufferDataFromVector(GL_ARRAY_BUFFER, birdMesh, GL_DYNAMIC_DRAW); // GL_DYNAMIC_DRAW GL_STATIC_DRAW

//	// Wait for fence (set below) before modifying buffer.
//	glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT,
//	                 GL_TIMEOUT_IGNORED);

////	 Modify buffer, flush, and unmap.
//	if (old_data) {
//		memcpy(old_data, birdMesh.data(), birdMesh.size() * sizeof(VERTEX));
//		glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, birdMesh.size() * sizeof(VERTEX));
//	}
//	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	// Pass the vertex data
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      BUFFER_OFFSET(0));
	glEnableVertexAttribArray(ATTRIB_VERTEX);

	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      BUFFER_OFFSET(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_NORMAL);

	glDrawArrays(GL_TRIANGLES, 0, birdMesh.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_);
//	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

}


bool Bird::decreaseInertion() {
	if (inertion > INERTION_MIN) {
		inertion-=INERTION_STEP;
		return true;
	}
	return false;
}



void Bird::onTap() {
	inertion = INERTION_MAX;
}
