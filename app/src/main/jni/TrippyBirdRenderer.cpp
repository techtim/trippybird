
//--------------------------------------------------------------------------------
// TrippyBirdRenderer.cpp
// Flappy Bird in 3D
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TrippyBirdRenderer.h"

//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
TrippyBirdRenderer::TrippyBirdRenderer()
		:bCameraActive(false),
		bGamePaused(true),
		bVerticalView(false),
		 gradientPos(0)
{
	setupWorld();
}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
TrippyBirdRenderer::~TrippyBirdRenderer() { Unload(); }

void TrippyBirdRenderer::Init() {
	// Settings

// Accept fragment if it closer to the camera than the former one


	// Load shader
	LoadShaders(&shader_param_, "Shaders/VS_ShaderPlain.vsh",
	            "Shaders/ShaderPlain.fsh");

	bird_.Init();
	cylinderObj_.Init();
	plane_.Init();
	UpdateViewport();
//    ndk_helper::Vec4 scale = ndk_helper::Vec4(10.f,10.f,10.f,1.f);
	mat_model_ = ndk_helper::Mat4::Translation(0, 0, 0);
	mat_view_ = ndk_helper::Mat4::LookAt(ndk_helper::Vec3(CAM_X, CAM_Y, CAM_Z),
	                                     ndk_helper::Vec3(0.f, .5f, 0.f),
	                                     ndk_helper::Vec3(0.f, 1.f, 0.f));
//	ndk_helper::Mat4 mat = ndk_helper::Mat4::RotationX(0);
	mat_view_ = mat_view_ * mat_model_;
}

void TrippyBirdRenderer::setupWorld() {
	srand(time(NULL));
	obstacles_dist = cylinderObj_.getRadius()*12;
	int num_obstacles = ceil((2.5f - 0.5)/obstacles_dist);
	obstacles_.clear();
	for (int i=0; i<num_obstacles; i++) {
		obstacles_.push_back(Obstacle(i*obstacles_dist));
		obstacles_.push_back(obstacles_[obstacles_.size()-1].createPair());
	}
	bird_.reset();

	materialCyl = {
			{.7f, 0.2f, 0.7f}, {1.0f, 1.0f, 7.0f, 10.f}, {0.4f, 0.0f, 0.6f}, };
	materialBird = {
			{.2f, 0.2f, 0.7f}, {1.0f, 1.0f, 7.0f, 10.f}, {0.2f, 0.4f, 0.6f}, };
}

void TrippyBirdRenderer::setPause(bool isPaused) {
	if (bGamePaused == true && !isPaused) {
		setupWorld();
	}
	bGamePaused = isPaused;
}

void TrippyBirdRenderer::UpdateViewport() {
	// Init Projection matrices
	int32_t viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

//	const float CAM_NEAR = 0.5f;
//	const float CAM_FAR = 100.f;
	if (viewport[2] < viewport[3]) {
		float aspect =
				static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
		mat_projection_ =
				ndk_helper::Mat4::Perspective(aspect, 1.0f, CAM_NEAR, CAM_FAR);
		bVerticalView = true;
	} else {
		float aspect =
				static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
		mat_projection_ =
				ndk_helper::Mat4::Perspective(1.0f, aspect, CAM_NEAR, CAM_FAR);
		bVerticalView = false;
	}
}

void TrippyBirdRenderer::Unload() {
	cylinderObj_.Unload();
	plane_.Unload();
	bird_.Unload();

	if (shader_param_.program_) {
		glDeleteProgram(shader_param_.program_);
		shader_param_.program_ = 0;
	}
}

void TrippyBirdRenderer::Update(float fTime) {
	if (bGamePaused) return;

	mat_view_ = ndk_helper::Mat4::LookAt(
				bVerticalView? ndk_helper::Vec3(CAM_VERT_X, CAM_VERT_Y, CAM_VERT_Z) : ndk_helper::Vec3(CAM_X, CAM_Y, CAM_Z),
								ndk_helper::Vec3(0.f, 0.5f, 0.f),
								ndk_helper::Vec3(0.f, 1.f, 0.f));

	bird_.update();
	if (bird_.getColliding()) setPause(true);

	for (auto it=obstacles_.begin(); it<obstacles_.end(); ) {
		it->update(fTime);
		if (intersects(bird_.getCircle(), it->getRect())) {
			bird_.setColliding(true);
			setPause(true);
		}
		if (it->getRect().x+it->getRect().width < -1.25f) {
			it = obstacles_.erase(it);
			it = obstacles_.erase(it);
			obstacles_.push_back(Obstacle(obstacles_[obstacles_.size()-1].getPositionX()+obstacles_dist));
			obstacles_.push_back(obstacles_[obstacles_.size()-1].createPair());
		} else {
			it++;
		}
	}

	gradientPos+=OBSTACLES_SPEED*0.1;
	if (gradientPos>=1.f) gradientPos=0;

	if (bCameraActive && camera_) {
		camera_->Update();
		mat_view_ = camera_->GetTransformMatrix() * mat_view_ *
		            camera_->GetRotationMatrix();
	} else {
		mat_view_ = mat_view_ * mat_model_;
    }
}

void TrippyBirdRenderer::Render() {
	//
	// Feed Projection and Model View matrices to the shaders
	glUseProgram(shader_param_.program_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update uniforms
	glUniform4f(shader_param_.material_diffuse_, materialCyl.diffuse_color[0],
	            materialCyl.diffuse_color[1], materialCyl.diffuse_color[2], 1.f);

	glUniform4f(shader_param_.material_specular_, materialCyl.specular_color[0],
	            materialCyl.specular_color[1], materialCyl.specular_color[2],
	            materialCyl.specular_color[3]);

	glUniform3f(shader_param_.material_ambient_, materialCyl.ambient_color[0],
	            materialCyl.ambient_color[1], materialCyl.ambient_color[2]);

	glUniform3f(shader_param_.light0_, bVerticalView? 25.f:-25.f, .5f, 25.f);

	// --- Draw walls

	float r,g,b;
	ndk_helper::Vec3 col = hueToRGB(gradientPos);
	col.Value(r,g,b);
	glUniform4f(shader_param_.color_gradient_1, r,g,b,1.f);
	col = hueToRGB(gradientPos+0.1);
	col.Value(r,g,b);
	glUniform4f(shader_param_.color_gradient_2, r,g,b,1.f);

	glUniform1i(shader_param_.object_type, TYPE_PLANE);
	ndk_helper::Mat4 mat_v = mat_view_ * ndk_helper::Mat4::Translation(0,0,0);
	ndk_helper::Mat4 mat_vp = mat_projection_ * mat_v;
	glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
	                   mat_vp.Ptr());
	glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
	plane_.draw();

	mat_v = mat_view_ * ndk_helper::Mat4::Translation(0,1,0);
	mat_vp = mat_projection_ * mat_v;
	glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
	                   mat_vp.Ptr());
	glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
	plane_.draw();

	mat_v = mat_view_ * ndk_helper::Mat4::Translation(0,0,0) * ndk_helper::Mat4::RotationX(90) * ndk_helper::Mat4::Translation(0,0,1.f);
	mat_vp = mat_projection_ * mat_v;
	glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
	                   mat_vp.Ptr());
	glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
	plane_.draw();


	// Draw Obstacles with Cylinders
	glUniform1i(shader_param_.object_type, TYPE_CYLINDER);
	cylinderObj_.bind();
	for (auto &obst:obstacles_ ) {
		mat_v = mat_view_ * obst.getModelMatrix();
		mat_vp = mat_projection_ * mat_v;
		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
		                   mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
		glUniform1i(shader_param_.object_type, TYPE_CYLINDER);
		cylinderObj_.draw();
	}

	cylinderObj_.unbind();

	// --- Draw Bird

	mat_v = mat_view_ * bird_.getModelMatrix();
	mat_vp = mat_projection_ * mat_v;

	glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
	                   mat_vp.Ptr());
	glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
	glUniform4f(shader_param_.material_diffuse_, materialBird.diffuse_color[0],
	            materialBird.diffuse_color[1], materialBird.diffuse_color[2], 1.f);

	glUniform4f(shader_param_.material_specular_, materialBird.specular_color[0],
	            materialBird.specular_color[1], materialBird.specular_color[2],
	            materialBird.specular_color[3]);

	glUniform3f(shader_param_.material_ambient_, materialBird.ambient_color[0],
	            materialBird.ambient_color[1], materialBird.ambient_color[2]);
//	glUniform1i(shader_param_.object_type, TYPE_BIRD);
	glUniform1i(shader_param_.object_type, TYPE_CYLINDER);
	bird_.draw();



	if (bCameraActive) {
		ndk_helper::Mat4 mat_v = mat_view_ * ndk_helper::Mat4::Translation(0,0,0);
		ndk_helper::Mat4 mat_vp = mat_projection_ * mat_v;

		glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
		                   mat_vp.Ptr());
		glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());
		glUniform1i(shader_param_.object_type, TYPE_LINE);
		drawAxis(1.f);
	}


}

bool TrippyBirdRenderer::LoadShaders(SHADER_PARAMS* params, const char* strVsh,
                                     const char* strFsh) {
	GLuint program;
	GLuint vert_shader, frag_shader;

	// Create shader program
	program = glCreateProgram();
	LOGI("Created Shader %d", program);

	// Create and compile vertex shader
	if (!ndk_helper::shader::CompileShader(&vert_shader, GL_VERTEX_SHADER,
	                                       strVsh)) {
		LOGI("Failed to compile vertex shader");
		glDeleteProgram(program);
		return false;
	}

	// Create and compile fragment shader
	if (!ndk_helper::shader::CompileShader(&frag_shader, GL_FRAGMENT_SHADER,
	                                       strFsh)) {
		LOGI("Failed to compile fragment shader");
		glDeleteProgram(program);
		return false;
	}

	// Attach vertex shader to program
	glAttachShader(program, vert_shader);

	// Attach fragment shader to program
	glAttachShader(program, frag_shader);

	// Bind attribute locations
	// this needs to be done prior to linking
	glBindAttribLocation(program, ATTRIB_VERTEX, "myVertex");
	glBindAttribLocation(program, ATTRIB_NORMAL, "myNormal");
	glBindAttribLocation(program, ATTRIB_UV, "myUV");

	// Link program
	if (!ndk_helper::shader::LinkProgram(program)) {
		LOGI("Failed to link program: %d", program);

		if (vert_shader) {
			glDeleteShader(vert_shader);
			vert_shader = 0;
		}
		if (frag_shader) {
			glDeleteShader(frag_shader);
			frag_shader = 0;
		}
		if (program) {
			glDeleteProgram(program);
		}

		return false;
	}

	// Get uniform locations
	params->matrix_projection_ = glGetUniformLocation(program, "uPMatrix");
	params->matrix_view_ = glGetUniformLocation(program, "uMVMatrix");

	params->light0_ = glGetUniformLocation(program, "vLight0");
	params->material_diffuse_ = glGetUniformLocation(program, "vMaterialDiffuse");
	params->material_ambient_ = glGetUniformLocation(program, "vMaterialAmbient");
	params->material_specular_ =
			glGetUniformLocation(program, "vMaterialSpecular");
	params->object_type = glGetUniformLocation(program, "iObjectType");
	params->color_gradient_1 = glGetUniformLocation(program, "colorGradient1");
	params->color_gradient_2 = glGetUniformLocation(program, "colorGradient2");
	// Release vertex and fragment shaders
	if (vert_shader) glDeleteShader(vert_shader);
	if (frag_shader) glDeleteShader(frag_shader);

	params->program_ = program;
	return true;
}

void TrippyBirdRenderer::onTap() {
	bird_.onTap();
}

void TrippyBirdRenderer::onDoubleTap() {
	bird_.onTap();
//	camera_->GetTransformMatrix() = ndk_helper::Mat4::Translation(.5,0.5,-1);
//	camera_->Reset(true);
//	bCameraActive = !bCameraActive;
}

bool TrippyBirdRenderer::Bind(ndk_helper::TapCamera* camera) {
	camera_ = camera;
	return true;
}
