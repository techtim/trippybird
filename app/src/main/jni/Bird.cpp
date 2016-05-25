//
// Created by Timofey Tavlintsev on 4/28/16.
//


#include "Bird.hpp"
//FlyingUpState Bird::flyingUp;
//FlyingDownState Bird::flyingDown;
Bird::Bird():
		inertion(0.f),
		bInit(false),
		bColliding(false)
{
	reset();
	circle.r=BIRD_HEIGHT/2;
	// Displace bounding circle of the bird
	world_pos_.Value(circle.x, circle.y, circle.z);
	circle.x -= BIRD_HEIGHT/2;
	circle.y += BIRD_HEIGHT/2;

	materials_ = { {.2f, 0.2f, 0.7f}, {1.0f, 1.0f, 7.0f, 10.f}, {0.2f, 0.4f, 0.6f} };

	setupGeometry();
}

void Bird::Init() {

	glGenBuffers(1, &vbo_);

	bInit = true;
}

void Bird::Unload() {
	if (vbo_) {
		glDeleteBuffers(1, &vbo_);
		vbo_ = 0;
	}
}

void Bird::update(float fTime) {


	if (!bColliding) {
		world_pos_ += Vec3(0, inertion, 0);

		animateWings(inertion);
		world_pos_.Value(circle.x, circle.y, circle.z);
		circle.x -= BIRD_HEIGHT / 2;
		circle.y += BIRD_HEIGHT / 2;
		decreaseInertion();
	} else {
		randomizeWings();
	}
}

void Bird::reset() {
	setupGeometry();
	world_pos_ = Vec3(BIRD_POS_X, 0.5, 0);
	bColliding = false;
}

void Bird::animateWings(float _inert) {
	birdMesh[1].pos[1] = BIRD_HEIGHT-_inert*4.f;
	birdMesh[1].pos[2] = BIRD_HEIGHT+_inert*2.f;
	birdMesh[1].normal[1] += _inert*4.f;

	birdMesh[4].pos[1] = BIRD_HEIGHT-_inert*4.f;
	birdMesh[4].pos[2] = -BIRD_HEIGHT-_inert*2.f;
	birdMesh[4].normal[1] -= _inert*4.f;
}

void Bird::randomizeWings() {
	for (auto &point:birdMesh) {
		point.pos[0] = BIRD_HEIGHT - (rand() % 10) * BIRD_HEIGHT / 5;
		point.pos[1] = BIRD_HEIGHT - (rand() % 10) * BIRD_HEIGHT / 5;
		point.pos[2] = BIRD_HEIGHT - (rand() % 10) * BIRD_HEIGHT / 5;
	}
}

void Bird::draw(const SHADER_PARAMS &shader_param_, const Mat4 &_view, const Mat4 &_projection) const {
	if (!bInit) {
		LOGI("[ERROR] Draw Bird without Init()");
		return;
	}

	// Set uniforms
	Mat4 mat_v = _view * Mat4::Translation(world_pos_);
	Mat4 mat_vp = _projection * mat_v;

	glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
	                   mat_vp.Ptr());
	glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
	glUniform4f(shader_param_.material_diffuse_, materials_.diffuse_color[0],
	            materials_.diffuse_color[1], materials_.diffuse_color[2], 1.f);

	glUniform4f(shader_param_.material_specular_, materials_.specular_color[0],
	            materials_.specular_color[1], materials_.specular_color[2],
	            materials_.specular_color[3]);

	glUniform3f(shader_param_.material_ambient_, materials_.ambient_color[0],
	            materials_.ambient_color[1], materials_.ambient_color[2]);

	// Draw bird with cylinder shader way for now
	//	glUniform1i(shader_param_.object_type, TYPE_BIRD);
	glUniform1i(shader_param_.object_type, TYPE_CYLINDER);

	// Bind and map buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	glBufferDataFromVector(GL_ARRAY_BUFFER, birdMesh, GL_DYNAMIC_DRAW); // GL_DYNAMIC_DRAW GL_STATIC_DRAW

	// Pass the vertex data
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      BUFFER_OFFSET(0));
	glEnableVertexAttribArray(ATTRIB_VERTEX);

	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      BUFFER_OFFSET(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_NORMAL);

	glDrawArrays(GL_TRIANGLES, 0, birdMesh.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Bird::setupGeometry() {
	birdMesh.resize(6);

	Vec3 u,v,normal;
	vertices.clear();
	normals.clear();
	vertices.push_back(Vec3(0, 0, 0));
	vertices.push_back(Vec3(-BIRD_LENGTH, BIRD_HEIGHT, BIRD_HEIGHT));
	vertices.push_back(Vec3(-BIRD_LENGTH, 0, 0));
//	u = vertices[1]-vertices[0];
//	v = vertices[2]-vertices[0];
//	normal = u.Cross(v);
	normal = Vec3(0, 1, -1.f);
	normals.push_back(normal); normals.push_back(normal); normals.push_back(normal);
	vertices.push_back(Vec3(0, 0, 0));
	vertices.push_back(Vec3(-BIRD_LENGTH, BIRD_HEIGHT, -BIRD_HEIGHT));
	vertices.push_back(Vec3(-BIRD_LENGTH, 0, 0));
//	u = vertices[4]-vertices[3];
//	v = vertices[5]-vertices[3];
//	normal = u.Cross(v);
	normal = Vec3(0, 1, 1.f);
	normals.push_back(normal); normals.push_back(normal); normals.push_back(normal);

	birdMesh.reserve(vertices.size());
	for (uint8_t i=0; i<vertices.size();i++) {
		vertices[i].Value(birdMesh[i].pos[0], birdMesh[i].pos[1],birdMesh[i].pos[2]);
		normals[i].Value(birdMesh[i].normal[0], birdMesh[i].normal[1],birdMesh[i].normal[2]);
	}
}