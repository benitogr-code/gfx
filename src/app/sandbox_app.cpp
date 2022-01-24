#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _inputFlags(0)
  , _mousePosition(0.0f, 0.0f)
  , _time(0.0f) {

}

bool SandboxApp::onInit() {
  auto matDefault = getAssetManager()->getDefaultMaterial();
  auto matBox = getAssetManager()->getMaterial("toy_box");

  _box.attachModel(GfxModel::Create(MeshUtils::CreateCube(2.0f), matBox));
  _box.setPosition(glm::vec3(3.0f, 1.5f, 0.5f));

  _ground.attachModel(GfxModel::Create(MeshUtils::CreateGroundPlane(2.0f, 50), matDefault));
  _cyborg.attachModel(getAssetManager()->loadModel("models/cyborg.gfx"));
  _cyborg.setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));

  _pointLight.attachModel(getAssetManager()->loadModel("models/point_light.gfx"));
  _pointLight.cloneModelMaterial();
  _pointLight.getModelMaterial()->setParamVec3("material.color", ColorRGB(0.20f, 0.80f, 0.63f));
  _pointLight.setPosition(glm::vec3(0.0f, 5.0f, 0.75f));
  _pointLight.setScale(glm::vec3(0.1f));

  Light::Properties props;
  props.color = ColorRGB(0.20f, 0.80f, 0.63f);
  props.ambientMultiplier = 0.1f;
  props.specularMultiplier = 1.2f;
  props.attenuationConstant = 0.01f;
  props.attenuationLinear = 0.15f;
  props.attenuationQuadratic = 0.07f;
  _pointLight.attachLight(props);

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
  _time += ctx.frameTime;

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

  auto& renderer = *getRenderer();
  auto& viewCamera = renderer.getViewCamera();
  viewCamera.setWorldLocation(_camera.position, _camera.getQuat());
  viewCamera.setFov(_camera.fov);

  // Render scene
  _pointLight.setPosition(glm::vec3(sinf(_time)*4.5f, 5.0f, 0.75f));
  _pointLight.render(renderer);

  _cyborg.render(renderer);
  _box.render(renderer);
  _ground.render(renderer);
}

void SandboxApp::onGUI() {
  // Settings ///
  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));

  if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::CollapsingHeader("Scene lighting")){
      Light& mainLight = getRenderer()->getMainLight();

      ImGui::ColorEdit3("Color", glm::value_ptr(mainLight.properties.color));
      ImGui::SliderFloat("Ambient mult", &mainLight.properties.ambientMultiplier, 0.05f, 0.4f);
      ImGui::SliderFloat("Specular mult", &mainLight.properties.specularMultiplier, 0.3f, 1.5f);
      ImGui::SliderFloat("Position", &mainLight.position.x, -200.0f, 200.0f);
    }

    if (ImGui::CollapsingHeader("Camera")) {
      ImGui::SliderFloat("Fov", &_camera.fov, 45.0f, 90.0f);
      ImGui::SliderFloat("Speed", &_camera.movementSpeed, 0.1f, 10.0f);
      ImGui::SliderFloat("Mouse sensitivity", &_camera.mouseSensitivity, 0.01f, 0.3f);
    }

    if (ImGui::CollapsingHeader("Debug")) {
      if (ImGui::Button("Toggle wireframe")) {
        getRenderer()->toggleWireframe();
      }
    }

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
