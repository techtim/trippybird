//
// Created by Timofey Tavlintsev on 4/28/16.
//


#include "Bird.hpp"
//FlyingUpState Bird::flyingUp;
//FlyingDownState Bird::flyingDown;
Bird::Bird():
		state_(FLYING_DOWN),
		world_pos(ndk_helper::Vec3(0.05, 0.5, 0))
{

}

void Bird::Init() {
//    state_ = &Bird::flyingUp;
	wingLeft.resize(3);

	wingLeft[0].pos[0] = wingLeft[0].pos[1] = wingLeft[0].pos[2] =0;
	wingLeft[1].pos[0] = 0.1;
	wingLeft[1].pos[1] = 0.1;
	wingLeft[1].pos[2] = -0.1;
	wingLeft[2].pos[0] = 0.13;
	wingLeft[2].pos[1] = 0;
	wingLeft[2].pos[2] = 0;
	ndk_helper::Vec3 u = ndk_helper::Vec3(wingLeft[1].pos[0],wingLeft[1].pos[1],wingLeft[1].pos[2])
	                     -ndk_helper::Vec3(wingLeft[0].pos[0],wingLeft[0].pos[1],wingLeft[0].pos[2]);
	ndk_helper::Vec3 v = ndk_helper::Vec3(wingLeft[2].pos[0],wingLeft[2].pos[2],wingLeft[2].pos[2])
	                     -ndk_helper::Vec3(wingLeft[0].pos[0],wingLeft[0].pos[1],wingLeft[0].pos[2]);
	ndk_helper::Vec3 normal = u.Cross(v);
	normal.Value(wingLeft[0].normal[0], wingLeft[0].normal[1], wingLeft[0].normal[2]);
	normal.Value(wingLeft[1].normal[0], wingLeft[1].normal[1], wingLeft[1].normal[2]);
	normal.Value(wingLeft[2].normal[0], wingLeft[2].normal[1], wingLeft[2].normal[2]);

//	wingLeft[0] = ndk_helper::Vec3(0, 0, 0);
//	wingLeft[1] = ndk_helper::Vec3(0.1, 0.1, -0.1);
//	wingLeft[2] = ndk_helper::Vec3(0.13, 0, 0);
	wingRight.resize(3);
	normal = ndk_helper::Vec3(0, 0, 0).Cross(ndk_helper::Vec3(0.13, 0, 0));
	wingRight[0].pos[0] = wingLeft[0].pos[1] = wingLeft[0].pos[2] =0;
	wingRight[1].pos[0] = 0.1;
	wingRight[1].pos[1] = 0.1;
	wingRight[1].pos[2] = -0.1;
	wingRight[2].pos[0] = 0.13;
	wingRight[2].pos[1] = 0;
	wingRight[2].pos[2] = 0;
	u = ndk_helper::Vec3(wingRight[1].pos[0],wingRight[1].pos[1],wingRight[1].pos[2])
	    -ndk_helper::Vec3(wingRight[0].pos[0],wingRight[0].pos[1],wingRight[0].pos[2]);
	v = ndk_helper::Vec3(wingRight[2].pos[0],wingRight[2].pos[2],wingRight[2].pos[2])
	    -ndk_helper::Vec3(wingRight[0].pos[0],wingRight[0].pos[1],wingRight[0].pos[2]);
	normal = u.Cross(v);
	normal.Value(wingRight[0].normal[0], wingRight[0].normal[1], wingRight[0].normal[2]);
	normal.Value(wingRight[1].normal[0], wingRight[1].normal[1], wingRight[1].normal[2]);
	normal.Value(wingRight[2].normal[0], wingRight[2].normal[1], wingRight[2].normal[2]);

//	wingRight[0] = ndk_helper::Vec3(0, 0, 0);
//	wingRight[1] = ndk_helper::Vec3(0.1, 0.1, 0.1);
//	wingRight[2] = ndk_helper::Vec3(0.13, 0, 0);

	glGenBuffers(1, &vbo_left);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_left);
	glBufferDataFromVector(GL_ARRAY_BUFFER, wingLeft, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &vbo_right);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_right);
	glBufferDataFromVector(GL_ARRAY_BUFFER, wingRight, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	inertion = 0;
	circle.r = 0.05;
	world_pos.Value(circle.x, circle.y, circle.z);
}

void Bird::Unload() {
	if (vbo_left) {
		glDeleteBuffers(1, &vbo_left);
		vbo_left = 0;
	}
	if (vbo_right) {
		glDeleteBuffers(1, &vbo_right);
		vbo_right = 0;
	}
	if (ibo_) {
		glDeleteBuffers(1, &ibo_);
		ibo_ = 0;
	}
}

void Bird::update() {
//    world_pos.y += inertion;
	decreaseInertion();
	world_pos.Value(circle.x, circle.y, circle.z);
	mat_model = ndk_helper::Mat4::Translation(circle.x, circle.y, circle.z);
}

void Bird::draw() {
//	glColor4f(0.8f,0.8f,0.8f,1.f);
//    ofSetColor(200);
//    ofDrawTriangle(0, 0, 0, 10, 10, 0, 0, 10, 0);
//    #ifndef TARGET_OPENGLES
//        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glEnableClientState(GL_VERTEX_ARRAY);

//	glPushMatrix();
//	glTranslatef(world_pos[0], world_pos.y, world_pos.z);
//	glPushMatrix();
//	glTranslatef(0,0,0);
//	glRotatef((inertion-INERTION_MIN)*1440.f, 1, 0, 0);

//	glVertexPointer(3, GL_FLOAT, sizeof(ndk_helper::Vec3), &wingRight[0]);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//	glPopMatrix();

//	glPushMatrix();
//	glTranslatef(0,0,0);
//	glRotatef((-inertion+INERTION_MIN)*1440.f, 1, 0, 0);

//	glVertexPointer(3, GL_FLOAT, sizeof(ndk_helper::Vec3), &wingLeft[0]);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//	glPopMatrix();


//	glPopMatrix();
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
