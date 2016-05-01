
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
TrippyBirdRenderer::TrippyBirdRenderer() {}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
TrippyBirdRenderer::~TrippyBirdRenderer() { Unload(); }

void TrippyBirdRenderer::Init() {
  // Settings
//  glFrontFace(GL_CW);
  glDisable(GL_CULL_FACE);
//    glCullFace(GL_FRONT_AND_BACK);
  // Load shader
  LoadShaders(&shader_param_, "Shaders/VS_ShaderPlain.vsh",
              "Shaders/ShaderPlain.fsh");

  srand(time(NULL));
  cylinderObj_.Init();

  obstacles_dist = cylinderObj_.getRadius()*8;
  int num_obstacles = ceil((2.f - 0.5)/obstacles_dist);
  for (int i=0; i<num_obstacles; i++) {
    obstacles_.push_back(Obstacle(i*obstacles_dist));
  }
  UpdateViewport();
//    ndk_helper::Vec4 scale = ndk_helper::Vec4(10.f,10.f,10.f,1.f);
  mat_model_ = ndk_helper::Mat4::Translation(0, 0, 0);

  ndk_helper::Mat4 mat = ndk_helper::Mat4::RotationX(0);
  mat_model_ = mat * mat_model_;
}

void TrippyBirdRenderer::UpdateViewport() {
  // Init Projection matrices
  int32_t viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  const float CAM_NEAR = 0.5f;
  const float CAM_FAR = 100.f;
  if (viewport[2] < viewport[3]) {
    float aspect =
            static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =
            ndk_helper::Mat4::Perspective(aspect, 1.0f, CAM_NEAR, CAM_FAR);
  } else {
    float aspect =
            static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
    mat_projection_ =
            ndk_helper::Mat4::Perspective(1.0f, aspect, CAM_NEAR, CAM_FAR);
  }
}

void TrippyBirdRenderer::Unload() {

  if (shader_param_.program_) {
    glDeleteProgram(shader_param_.program_);
    shader_param_.program_ = 0;
  }
}

void TrippyBirdRenderer::Update(float fTime) {
  const float CAM_X = 1.f;
  const float CAM_Y = 0.5f;
  const float CAM_Z = 2.f;

  mat_view_ = ndk_helper::Mat4::LookAt(ndk_helper::Vec3(CAM_X, CAM_Y, CAM_Z),
                                       ndk_helper::Vec3(0.f, 0.f, 0.f),
                                       ndk_helper::Vec3(0.f, 1.f, 0.f));

  for (auto it=obstacles_.begin(); it<obstacles_.end(); ) {

    it->Update(fTime);
    if (it->getRect().x+it->getRect().width < 0) {
      it = obstacles_.erase(it);
      obstacles_.push_back(Obstacle(obstacles_[obstacles_.size()-1].getPositionX()+obstacles_dist));
    } else {
      it++;
    }
  }

  if (camera_) {
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

  CYLINDER_MATERIALS material = {
          {5.0f, 0.7f, 0.1f}, {1.0f, 1.0f, 7.0f, 10.f}, {0.1f, 0.1f, 0.1f}, };

  // Update uniforms
  glUniform4f(shader_param_.material_diffuse_, material.diffuse_color[0],
              material.diffuse_color[1], material.diffuse_color[2], 1.f);

  glUniform4f(shader_param_.material_specular_, material.specular_color[0],
              material.specular_color[1], material.specular_color[2],
              material.specular_color[3]);
  //
  // using glUniform3fv here was troublesome
  //
  glUniform3f(shader_param_.material_ambient_, material.ambient_color[0],
              material.ambient_color[1], material.ambient_color[2]);

  glUniform3f(shader_param_.light0_, 100.f, -200.f, -600.f);


  cylinderObj_.bind();
  for (auto &obst:obstacles_ ) {


    ndk_helper::Mat4 mat_v = mat_view_ * obst.getModelMatrix();
    ndk_helper::Mat4 mat_vp = mat_projection_ * mat_v;

    glUniformMatrix4fv(shader_param_.matrix_projection_, 1, GL_FALSE,
                       mat_vp.Ptr());
    glUniformMatrix4fv(shader_param_.matrix_view_, 1, GL_FALSE, mat_v.Ptr());

    cylinderObj_.draw();
  }
  cylinderObj_.unbind();


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

  // Release vertex and fragment shaders
  if (vert_shader) glDeleteShader(vert_shader);
  if (frag_shader) glDeleteShader(frag_shader);

  params->program_ = program;
  return true;
}

bool TrippyBirdRenderer::Bind(ndk_helper::TapCamera* camera) {
  camera_ = camera;
  return true;
}