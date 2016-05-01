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
	wingLeft[0] = ndk_helper::Vec3(0, 0, 0);
	wingLeft[1] = ndk_helper::Vec3(0.1, 0.1, -0.1);
	wingLeft[2] = ndk_helper::Vec3(0.13, 0, 0);
	wingRight.resize(3);
	wingRight[0] = ndk_helper::Vec3(0, 0, 0);
	wingRight[1] = ndk_helper::Vec3(0.1, 0.1, 0.1);
	wingRight[2] = ndk_helper::Vec3(0.13, 0, 0);
	inertion = 0;
	circle.r = 0.05;
	world_pos.Value(circle.x, circle.y, circle.z);
}

void Bird::update() {
//    world_pos.y += inertion;
	decreaseInertion();
	world_pos.Value(circle.x, circle.y, circle.z);
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
