#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _bgColor(0.2f, 0.2f, 0.2f)
  , _inputFlags(0)
  , _mousePosition(0.0f, 0.0f)
  , _planetOffset(0.0f) {

}

bool SandboxApp::onInit() {
  MeshMaterial material;
  material.color = glm::vec3(0.5f, 0.0f, 0.0f);
  material.ambientFactor = 0.05f;
  material.specularFactor = 0.6f;

  _mesh = MeshUtils::CreateCube(1.75f);
  _mesh->setMaterial(material);

  _model = Model3D::Create("objects/planet/planet.obj");

  ShaderCreateParams shaderParams;
  shaderParams.name = "illum_pong";
  shaderParams.vertexShaderPath = "shaders/illum_pong.vert";
  shaderParams.fragmentShaderPath = "shaders/illum_pong.frag";
  _shaderIllumPong = Shader::Create(shaderParams);

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  const bool released = (event.state == InputState_Released);

  if (event.keyId == KeyId_Left) {
    setInputFlag(InputFlag_MoveLeft, !released);
  }
  else if (event.keyId == KeyId_Right) {
    setInputFlag(InputFlag_MoveRight, !released);
  }
  else if (event.keyId == KeyId_Up) {
    setInputFlag(InputFlag_MoveForward, !released);
  }
  else if (event.keyId == KeyId_Down) {
    setInputFlag(InputFlag_MoveBackward, !released);
  }
}

void SandboxApp::onMouseEvent(const MouseEvent& event) {
  const bool released = (event.state == InputState_Released);

  if (event.button == MouseButton_Left) {
    setInputFlag(InputFlag_MouseLeft, !released);
  }
  else if (event.button == MouseButton_Right) {
    setInputFlag(InputFlag_MouseRight, !released);
  }
  else if (event.button == MouseButton_Motion) {
    if(hasInputFlag(InputFlag_MouseLeft)) {
      const float xoffset = (float)(event.pos.x - _mousePosition.x) * _camera.mouseSensitivity;
      const float yoffset = (float)(_mousePosition.y - event.pos.y) * _camera.mouseSensitivity;

      _camera.yaw -= xoffset;
      _camera.pitch = std::max(-89.0f, std::min(_camera.pitch + yoffset, 89.0f));

      _camera.updateAxis();
    }

    _mousePosition = event.pos;
  }
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  // Update camera
  const float cameraSpeed = _camera.movementSpeed * ctx.frameTime;
  if (hasInputFlag(InputFlag_MoveLeft)) {
    _camera.position -= (_camera.right * cameraSpeed);
  }
  if (hasInputFlag(InputFlag_MoveRight)) {
    _camera.position += (_camera.right * cameraSpeed);
  }
  if (hasInputFlag(InputFlag_MoveForward)) {
    _camera.position += (_camera.forward * cameraSpeed);
  }
  if (hasInputFlag(InputFlag_MoveBackward)) {
    _camera.position -= (_camera.forward * cameraSpeed);
  }

  _camera.updateAxis();

  getRenderer()->getViewCamera().setWorldLocation(_camera.position, _camera.getQuat());

  // Render scene
  getRenderer()->setClearColor(_bgColor);

  const glm::vec3 planetPos(3.0f, _planetOffset, 3.0f);
  const ColorRGB  lightColor(1.0f, 1.0f, 1.0f);

  _shaderIllumPong->use();
  _shaderIllumPong->setUniformVec3("view_pos", _camera.position);
  _shaderIllumPong->setUniformMatrix4("mtx_viewProj", getRenderer()->getViewCamera().getViewProjection());
  _shaderIllumPong->setUniformVec3("light_pos", planetPos);
  _shaderIllumPong->setUniformVec3("light_color", lightColor);

  _shaderIllumPong->setUniformMatrix4("mtx_model", glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f)));
  getRenderer()->draw(_mesh, _shaderIllumPong);

  _shaderIllumPong->setUniformMatrix4("mtx_model", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.6f)), planetPos));
  getRenderer()->draw(_model, _shaderIllumPong);
}

void SandboxApp::onGUI() {
  static int counter = 0;

  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
  //ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
  ImGui::Begin("Settings", nullptr);
    ImGui::ColorEdit3("Background color", glm::value_ptr(_bgColor));

    if (ImGui::Button("Toggle wireframe")) {
      getRenderer()->toggleWireframe();
    }

    ImGui::SliderFloat("Light offset", &_planetOffset, -5.0f, 5.0f);

    ImGui::Spacing();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void SandboxApp::setInputFlag(uint32_t flag, bool set) {
  _inputFlags = set ? (_inputFlags | flag) : (_inputFlags & ~flag);
}

bool SandboxApp::hasInputFlag(uint32_t flag) const {
  return (_inputFlags & flag) != 0;
}
