#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _inputFlags(0)
  , _mousePosition(0.0f, 0.0f)
  , _time(0.0f) {

}

bool SandboxApp::onInit() {
  auto floorMaterial = getAssetManager()->getMaterial("stone_floor");
  auto skyboxMaterial = getAssetManager()->getMaterial("skybox");

  _boxes[0].attachModel(getAssetManager()->loadModel("models/wooden_crate.gfx"));
  _boxes[0].setPosition(glm::vec3(3.0f, 1.0f, 0.5f));
  _boxes[0].setRotation(glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

  _boxes[1].attachModel(getAssetManager()->loadModel("models/wooden_crate.gfx"));
  _boxes[1].setPosition(glm::vec3(-3.0f, 1.0f, 0.5f));

  _ground.attachModel(GfxModel::Create(MeshUtils::CreateGroundPlane(2.0f, 15, 2.0f), floorMaterial));
  _skybox.attachModel(GfxModel::Create(MeshUtils::CreateSkybox(), skyboxMaterial));

  _cyborg.attachModel(getAssetManager()->loadModel("models/cyborg.gfx"));
  _cyborg.setPosition(glm::vec3(0.0f, 0.2f, -0.7f));

  ColorRGB colors[2] = { ColorRGB(0.20f, 0.80f, 0.63f), ColorRGB(0.5f, 0.2f, 0.1f) };
  for (int i = 0; i < 2; ++i) {
    _pointLights[i].attachModel(getAssetManager()->loadModel("models/point_light.gfx"));
    _pointLights[i].cloneModelMaterial();
    _pointLights[i].getModelMaterial()->setParamVec3("material.color", colors[i]);
    _pointLights[i].setScale(glm::vec3(0.1f));

    Light::Properties props;
    props.color = colors[i];
    props.ambientMultiplier = 0.1f;
    props.specularMultiplier = 1.2f;
    props.attenuationConstant = 0.01f;
    props.attenuationLinear = 0.1f;
    props.attenuationQuadratic = 0.05f;
    _pointLights[i].attachLight(props);
  }

  _camera.position = glm::vec3(-1.2f, 3.44f, 5.71f);
  _camera.pitch = -18.8f;
  _camera.yaw = -6.6f;

  _mainLightLat = 50.0f;
  _mainLightLong = 265.0f;

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  const bool pressed = (event.state == InputState_Pressed);
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

  if (pressed && event.key == KeyboardKey_1) {
    const bool hide = _pointLights[0].hasFlag(Entity::Flags::Hidden);
    _pointLights[0].setFlag(Entity::Flags::Hidden, !hide);
  }
  if (pressed && event.key == KeyboardKey_2) {
    const bool hide = _pointLights[1].hasFlag(Entity::Flags::Hidden);
    _pointLights[1].setFlag(Entity::Flags::Hidden, !hide);
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

  const float latitude = glm::radians(_mainLightLat);
  const float longitude = glm::radians(_mainLightLong);

  glm::vec3 sunPosition;
  sunPosition.x = 1000.0f * cosf(latitude) * cosf(longitude);
  sunPosition.y = 1000.0f * sinf(latitude);
  sunPosition.z = 1000.0f * cosf(latitude) * sinf(longitude);

  renderer.getMainLight().position = sunPosition;

  // Render scene
  _pointLights[0].setPosition(glm::vec3(sinf(_time)*4.0f, 4.5f, cosf(_time)*4.0f));
  _pointLights[0].render(renderer);

  _pointLights[1].setPosition(glm::vec3(-sinf(_time)*3.5f, 5.0f, cosf(_time)*3.5f));
  _pointLights[1].render(renderer);

  _ground.render(renderer);

  _boxes[0].render(renderer);
  _boxes[1].render(renderer);

  _cyborg.setRotation(glm::angleAxis(_time*0.7f, glm::vec3(0.0f, 1.0f, 0.0f)));
  _cyborg.render(renderer);

  _skybox.render(renderer);
}

void SandboxApp::onGUI() {
  // Settings ///
  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));

  if (ImGui::Begin("Settings", nullptr)) {
    if (ImGui::CollapsingHeader("Main Light", ImGuiTreeNodeFlags_DefaultOpen)){
      Light& mainLight = getRenderer()->getMainLight();

      ImGui::ColorEdit3("Color", glm::value_ptr(mainLight.properties.color));
      ImGui::SliderFloat("Ambient", &mainLight.properties.ambientMultiplier, 0.05f, 0.4f);
      ImGui::SliderFloat("Specular", &mainLight.properties.specularMultiplier, 0.3f, 1.5f);
      ImGui::SliderFloat("Position Lon", &_mainLightLong, 0.0f, 360.0f);
      ImGui::SliderFloat("Position Lat", &_mainLightLat, -5.0f, 85.0f);
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
