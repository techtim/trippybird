
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
gradientPos(0),
lagTime(0)
{
	// Setup bounding boxes for top and buttom planes
	topPlaneRect.x = bottomPlaneRect.x = -1.f;
	topPlaneRect.z = bottomPlaneRect.z = 0;
	topPlaneRect.y = 1.f;
	bottomPlaneRect.y = 0.f;
	bottomPlaneRect.height = topPlaneRect.height = 0;
	topPlaneRect.width = bottomPlaneRect.width = 2.f;

	mat_model_ = Mat4::Translation(0, 0, 0);
	mat_view_ = Mat4::LookAt(Vec3(CAM_X, CAM_Y, CAM_Z),
	                         Vec3(0.f, .5f, 0.f),
	                         Vec3(0.f, 1.f, 0.f));
	mat_view_ = mat_view_ * mat_model_;
}


// Dtor
TrippyBirdRenderer::~TrippyBirdRenderer() { Unload(); }

void TrippyBirdRenderer::Init() {

	// Load shader
	LoadShaders(&shader_param_, "Shaders/VS_ShaderPlain.vsh",
	            "Shaders/ShaderPlain.fsh");

	// Init objects for GL
	bird_.Init();
	cylinderObj_ = std::shared_ptr<CylinderObject>(new CylinderObject());
	cylinderObj_->Init();
	plane_.Init();

	UpdateViewport();
	setupWorld();

	currentTime = PerfMonitor::GetCurrentTime();
}

void TrippyBirdRenderer::setupWorld() {

	srand(time(NULL));

	// Setup Obstacles
	obstacles_dist = SPACE_BETWEEN_OBSTACLES;
	num_obstacles = static_cast<size_t>(ceil(4.f/obstacles_dist));
	obstacles_.clear();
	for (size_t i=0; i<num_obstacles; i++) {
		obstacles_.push_back(Obstacle(i*obstacles_dist));
		float grad_ofset = i*obstacles_dist/4.f;
//		float grad_ofset = i*obstacles_dist/num_obstacles*4.f;
		UpdateCylinderMaterial(materialCyl, gradientPos+grad_ofset);
		obstacles_[obstacles_.size()-1].setDrawableObject(cylinderObj_);
		obstacles_[obstacles_.size()-1].setMaterials(materialCyl);
		obstacles_.push_back(obstacles_[obstacles_.size()-1].createPair());
	}

	obstacles_speed = OBSTACLES_SPEED;
	bird_.reset();

	lagTime = 0;
}

void TrippyBirdRenderer::setPause(bool isPaused) {

	// reset world if game was paused and then unpaused
	if (bGamePaused && !isPaused) {
		setupWorld();
	}
	bGamePaused = isPaused;
}

void TrippyBirdRenderer::UpdateViewport() {

	// Init Projection matrices
	int32_t viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (viewport[2] < viewport[3]) {
		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
		mat_projection_ = Mat4::Perspective(aspect, 1.0f, CAM_NEAR, CAM_FAR);
		bVerticalView = true;
	} else {
		float aspect = static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
		mat_projection_ = Mat4::Perspective(1.0f, aspect, CAM_NEAR, CAM_FAR);
		bVerticalView = false;
	}
}

void TrippyBirdRenderer::Unload() {

	cylinderObj_->Unload();
	plane_.Unload();
	bird_.Unload();

	if (shader_param_.program_) {
		glDeleteProgram(shader_param_.program_);
		shader_param_.program_ = 0;
	}
}

void TrippyBirdRenderer::Update(double fTime) {
	// Count and update using a fixed time step
	elapsedTime = fTime - currentTime;
	currentTime = fTime;

	// --- Update View Mat
	mat_view_ = Mat4::LookAt(
			bVerticalView? Vec3(CAM_VERT_X, CAM_VERT_Y, CAM_VERT_Z) : Vec3(CAM_X, CAM_Y, CAM_Z),
			Vec3(0.f, 0.5f, 0.f),
			Vec3(0.f, 1.f, 0.f));
	if (bCameraActive && camera_) {
		camera_->Update();
		mat_view_ = camera_->GetTransformMatrix() * mat_view_ * camera_->GetRotationMatrix();
	} else {
		mat_view_ = mat_view_ * mat_model_;
	}

	// --- Don't update World if paused
	if (bGamePaused) return;

	// Update
	lagTime += elapsedTime;
	while (lagTime >=SEC_PER_UPDATE) {
		lagTime -= SEC_PER_UPDATE;
		// --- Update bird
		bird_.update(fTime);

		gradientPos += obstacles_speed*0.2;
		if (gradientPos > 1.f) gradientPos = 0;

		// --- Check if collide with roof or floor
		if (intersects(bird_.getCircle(), topPlaneRect) ||
		    intersects(bird_.getCircle(), bottomPlaneRect)) {
			bird_.setColliding(true);
			bird_.update(fTime);
			setPause(true);
			return;
		}

		// --- Update Obstacles and check for collide with bird
		for (auto it = obstacles_.begin(); it < obstacles_.end();) {
			it->update(obstacles_speed);
			if (intersects(bird_.getCircle(), it->getRect())) {
				bird_.setColliding(true);
				bird_.update(fTime);
				setPause(true);
				return;
			}
			else if (it->getRect().x + it->getRect().width < -1.25f) {
				it = obstacles_.erase(it);
				it = obstacles_.erase(it);
				obstacles_.push_back(Obstacle(obstacles_[obstacles_.size() - 1].getPositionX() + obstacles_dist));
				UpdateCylinderMaterial(materialCyl, //gradientPos+0.1);
				                       gradientPos +((num_obstacles- 1)*obstacles_dist/4.f));
				obstacles_[obstacles_.size() - 1].setMaterials(materialCyl);
				obstacles_[obstacles_.size() - 1].setDrawableObject(cylinderObj_);
				obstacles_.push_back(obstacles_[obstacles_.size() - 1].createPair());

				// increase speed with each obstacle passed
				obstacles_speed += 0.0001;
			} else {
				it++;
			}
		}
	}
}

void TrippyBirdRenderer::Render() {
	//
	// --- Bind shader
	glUseProgram(shader_param_.program_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// --- Update uniforms
	glUniform3f(shader_param_.light0_, LIGHT_X, LIGHT_Y, LIGHT_Z);

	// Generate gradient from hue with offset
	float r, g, b;
	Vec3 col = hueToRGB(gradientPos);
	col.Value(r, g, b);
	glUniform4f(shader_param_.color_gradient_1, r, g, b, 1.f);
	col = hueToRGB(gradientPos + 0.1);
	col.Value(r, g, b);
	glUniform4f(shader_param_.color_gradient_2, r, g, b, 1.f);

	// --- Draw walls
	plane_.draw(shader_param_, mat_view_, mat_projection_);

	// --- Draw Obstacles with Cylinders
//	cylinderObj_->bind();
	for (auto &obst:obstacles_ ) {
		obst.draw(shader_param_, mat_view_, mat_projection_);
	}
//	cylinderObj_->unbind();

	// --- Draw Bird
	bird_.draw(shader_param_, mat_view_, mat_projection_);

}

void TrippyBirdRenderer::UpdateCylinderMaterial(MATERIALS& mat, float hue_pos) {
	Vec3 color1 = hueToRGB(hue_pos);
	Vec3 color2 = hueToRGB(hue_pos+0.1);
	mat.ambient_color = {color1[0],color1[1],color1[2]};
	mat.diffuse_color = {color2[0],color2[1],color2[2]};
	mat.specular_color = {1.0f, 1.0f, .8f, 10.f};
}

bool TrippyBirdRenderer::LoadShaders(SHADER_PARAMS* params, const char* strVsh,
                                     const char* strFsh) {
	GLuint program;
	GLuint vert_shader, frag_shader;

	// Create shader program
	program = glCreateProgram();
	LOGI("Created Shader %d", program);

	// Create and compile vertex shader
	if (!shader::CompileShader(&vert_shader, GL_VERTEX_SHADER,
	                                       strVsh)) {
		LOGI("Failed to compile vertex shader");
		glDeleteProgram(program);
		return false;
	}

	// Create and compile fragment shader
	if (!shader::CompileShader(&frag_shader, GL_FRAGMENT_SHADER,
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
	if (!shader::LinkProgram(program)) {
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
//	camera_->GetTransformMatrix() = Mat4::Translation(.5,0.5,-1);
//	camera_->Reset(true);
//	bCameraActive = !bCameraActive;
}

bool TrippyBirdRenderer::Bind(TapCamera* camera) {
	camera_ = camera;
	return true;
}
