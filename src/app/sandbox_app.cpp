#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _bgColor(0.0f, 0.0f, 0.0f)
  , _inputFlags(0)
  , _mousePosition(0.0f, 0.0f)
  , _lightPos(0.0f, 4.0f, 2.0f)
  , _lightColor(1.0f, 1.0f, 1.0f) {

}

bool SandboxApp::onInit() {
  _light = MeshUtils::CreateCube(0.1f);

  _box = MeshUtils::CreateCube(2.0f);
  _ground = MeshUtils::CreateGroundPlane(2.0f, 50);

  _cyborg = Model3D::Create("objects/cyborg/cyborg.obj");

  _matDefault = getAssetManager()->getDefaultMaterial();
  _matLightSource = getAssetManager()->getDefaultMaterial();

  _camera.position = glm::vec3(-1.2f, 3.44f, 5.71f);
  _camera.pitch = -18.8f;
  _camera.yaw = -6.6f;

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  const bool released = (event.state == InputState_Released);

  if (event.key == KeyboardKey_A) {
    setInputFlag(InputFlag_MoveLeft, !released);
  }
  else if (event.key == KeyboardKey_D) {
    setInputFlag(InputFlag_MoveRight, !released);
  }
  else if (event.key == KeyboardKey_W) {
    setInputFlag(InputFlag_MoveForward, !released);
  }
  else if (event.key == KeyboardKey_S) {
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

  auto& viewCamera = getRenderer()->getViewCamera();
  viewCamera.setWorldLocation(_camera.position, _camera.getQuat());
  viewCamera.setFov(_camera.fov);

  // Render scene
  getRenderer()->setClearColor(_bgColor);

  // Hack
  _matLightSource->setParamVec3("mat_color", _lightColor);

  RenderItem light;
  light.mesh = _light;
  light.material = _matLightSource;
  light.modelTM = glm::translate(glm::mat4(1.0f), _lightPos);
  getRenderer()->draw(light);

  // Hack
  _matDefault->setParamVec3("light_pos", _lightPos);
  _matDefault->setParamVec3("light_color", _lightColor);

  for (uint32_t idx = 0; idx < _cyborg->getMeshCount(); ++idx) {
    RenderItem item;
    item.mesh = _cyborg->getMesh(idx);
    item.material = _cyborg->getMeshMaterial(idx);
    item.modelTM = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));

    getRenderer()->draw(item);
  }

  RenderItem box;
  box.mesh = _box;
  box.material = _matDefault;
  box.modelTM = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 0.0f));
  getRenderer()->draw(box);

  RenderItem ground;
  ground.mesh = _ground;
  ground.material = _matDefault;
  ground.modelTM = glm::mat4(1.0f);
  getRenderer()->draw(ground);
}

void SandboxApp::onGUI() {
  // Settings ///

  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
  //ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
  if (ImGui::Begin("Settings", nullptr)) {
    ImGui::BeginGroup();
      ImGui::Text("Renderer");
      ImGui::ColorEdit3("Background color", glm::value_ptr(_bgColor));
      ImGui::SliderFloat("Light offset", &_lightPos.x, -4.0f, 4.0f);
      ImGui::ColorEdit3("Light color", glm::value_ptr(_lightColor));
      if (ImGui::Button("Toggle wireframe")) {
        getRenderer()->toggleWireframe();
      }
    ImGui::EndGroup();

    ImGui::Spacing();

    ImGui::BeginGroup();
      ImGui::Text("Camera");
      ImGui::SliderFloat("Fov", &_camera.fov, 45.0f, 90.0f);
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
