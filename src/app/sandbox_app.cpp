#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _bgColor(0.5f, 0.6f, 0.7f)
  , _inputFlags(0)
  , _mousePosition(0.0f, 0.0f) {

}

bool SandboxApp::onInit() {
  MeshMaterial material;
  material.color = glm::vec3(0.5f, 0.0f, 0.0f);
  material.ambientFactor = 0.05f;
  material.specularFactor = 0.6f;

  _mesh = MeshUtils::CreateCube(1.75f);
  _mesh->setMaterial(material);

  material.color = glm::vec3(0.5f, 0.5f, 0.5f);
  material.ambientFactor = 0.075f;
  material.specularFactor = 0.4f;
  _ground = MeshUtils::CreateGroundPlane(2.0f, 50);
  _ground->setMaterial(material);

  _model = Model3D::Create("objects/planet/planet.obj");

  ShaderCreateParams shaderParams;
  shaderParams.name = "illum_pong";
  shaderParams.vertexShaderPath = "shaders/illum_pong.vert";
  shaderParams.fragmentShaderPath = "shaders/illum_pong.frag";
  _shaderIllumPong = Shader::Create(shaderParams);

  _camera.pitch = -20.0f;

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

  const glm::vec3 planetPos(3.0f, 3.5f, 3.0f);
  const ColorRGB  lightColor(1.0f, 1.0f, 1.0f);

  _shaderIllumPong->use();
  _shaderIllumPong->setUniformVec3("view_pos", getRenderer()->getViewCamera().getPosition());
  _shaderIllumPong->setUniformMatrix4("mtx_viewProj", getRenderer()->getViewCamera().getViewProjection());
  _shaderIllumPong->setUniformVec3("light_pos", planetPos);
  _shaderIllumPong->setUniformVec3("light_color", lightColor);

  _shaderIllumPong->setUniformMatrix4("mtx_model", glm::mat4(1.0f));
  getRenderer()->draw(_ground, _shaderIllumPong);

  _shaderIllumPong->setUniformMatrix4("mtx_model", glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.5f, 0.0f)));
  getRenderer()->draw(_mesh, _shaderIllumPong);

  _shaderIllumPong->setUniformMatrix4("mtx_model", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)), planetPos));
  getRenderer()->draw(_model, _shaderIllumPong);
}

void SandboxApp::onGUI() {
  // Settings ///

  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
  //ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
  if (ImGui::Begin("Settings", nullptr)) {
    ImGui::BeginGroup();
      ImGui::Text("Renderer");
      ImGui::ColorEdit3("Background color", glm::value_ptr(_bgColor));

      if (ImGui::Button("Toggle wireframe")) {
        getRenderer()->toggleWireframe();
      }
    ImGui::EndGroup();

    ImGui::Spacing();

    ImGui::BeginGroup();
      ImGui::Text("Camera");
      ImGui::SliderFloat("Speed", &_camera.movementSpeed, 0.1f, 10.0f);
      ImGui::SliderFloat("Mouse sensitivity", &_camera.mouseSensitivity, 0.01f, 0.3f);
    ImGui::EndGroup();

    ImGui::End();
  }

  // Top-Right overlay ///
  const float PAD = 10.0f;
  ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImVec2 workPos = viewport->WorkPos;
  ImVec2 workSize = viewport->WorkSize;
  ImVec2 windowPos, windowPosPivot;
  windowPos.x = workPos.x + workSize.x - PAD;
  windowPos.y = workPos.y + PAD;
  windowPosPivot.x = 1.0f;
  windowPosPivot.y = 0.0f;

  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
  ImGui::SetNextWindowBgAlpha(0.35f);
  if (ImGui::Begin("Overlay", nullptr, overlayFlags))
  {
    ImGui::Text(
      "Camera Position [%.3f, %.3f, %.3f] | Pitch %.2f, Yaw %.2f",
      _camera.position.x, _camera.position.y, _camera.position.z, _camera.pitch, _camera.yaw
    );
    ImGui::Text("Frame time %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
}

void SandboxApp::setInputFlag(uint32_t flag, bool set) {
  _inputFlags = set ? (_inputFlags | flag) : (_inputFlags & ~flag);
}

bool SandboxApp::hasInputFlag(uint32_t flag) const {
  return (_inputFlags & flag) != 0;
}
