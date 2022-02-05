#include "sandbox_app.h"
#include "demos/scene_cubemaps.h"
#include "demos/scene_playground.h"

#include <imgui.h>

#define SCENE_PLAYGROUND 0
#define SCENE_CUBEMAPS   1

SandboxApp::SandboxApp()
  : _inputFlags(0)
  , _mousePosition(0.0f, 0.0f)
  , _selectedScene(~0) {

}

bool SandboxApp::onInit() {
  changeScene(SCENE_PLAYGROUND);

  return true;
}

void SandboxApp::onShutdown() {
  _scene.reset();
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

  _scene->onInputEvent(event);
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
  else if (event.button == MouseButton_MouseWheel) {
    const float fov = _camera.fov - (event.pos.y * 0.5f);
    _camera.fov = std::clamp(fov, 30.0f, 90.0f);
  }
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  // Update
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

  _scene->update(ctx.frameTime);

  // Render
  auto& renderer = *getRenderer();
  auto& viewCamera = renderer.getViewCamera();
  viewCamera.setWorldLocation(_camera.position, _camera.getQuat());
  viewCamera.setFov(_camera.fov);

  _scene->render(renderer);
}

void SandboxApp::onGUI() {
  // App menu bar ///
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Demo Scene")){
      if (ImGui::MenuItem("Playground", nullptr, nullptr, _selectedScene != SCENE_PLAYGROUND)) {
        LOG_INFO("[App] Switching to playground scene");
        changeScene(SCENE_PLAYGROUND);
      }
      if (ImGui::MenuItem("Cubemaps", nullptr, nullptr, _selectedScene != SCENE_CUBEMAPS)) {
        LOG_INFO("[App] Switching to cubemaps scene");
        changeScene(SCENE_CUBEMAPS);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  // App settings ///
  ImGui::SetNextWindowPos(ImVec2(5.0f, ImGui::GetTextLineHeight() * 2.0f));
  if (ImGui::Begin("App settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::CollapsingHeader("Camera")) {
      ImGui::SliderFloat("Speed", &_camera.movementSpeed, 0.1f, 10.0f);
      ImGui::SliderFloat("Mouse sensitivity", &_camera.mouseSensitivity, 0.01f, 0.3f);
    }

    if (ImGui::CollapsingHeader("Debug")) {
      if (ImGui::Button("Toggle wireframe")) {
        getRenderer()->toggleWireframe();
      }
    }

    _scene->onGUI();

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
    auto& stats = getRenderer()->getStats();

    ImGui::Text(
      "Camera Position [%.3f, %.3f, %.3f] | Pitch %.2f, Yaw %.2f | Fov %.2f",
      _camera.position.x, _camera.position.y, _camera.position.z,
      _camera.pitch, _camera.yaw, _camera.fov
    );
    ImGui::Text("Drawcalls %d | Point lights %d", stats.drawcalls, stats.pointlights);
    ImGui::Text("Frame time %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
}

void SandboxApp::changeScene(uint32_t id) {
  if (_selectedScene == id) return;

  if (id == SCENE_PLAYGROUND) {
    _scene.reset(new ScenePlayground(*getAssetManager()));
    _scene->init();
    _selectedScene = id;

    _camera.position = glm::vec3(0.0f, 3.5f, 6.0f);
    _camera.pitch = -20.0f;
    _camera.yaw = 0.0f;
  }
  else if (id == SCENE_CUBEMAPS) {
    _scene.reset(new SceneCubemaps(*getAssetManager()));
    _scene->init();
    _selectedScene = id;

    _camera.position = glm::vec3(2.76f, 0.84f, 5.54f);
    _camera.pitch = 4.6f;
    _camera.yaw = 19.0f;
  }
}

void SandboxApp::setInputFlag(uint32_t flag, bool set) {
  _inputFlags = set ? (_inputFlags | flag) : (_inputFlags & ~flag);
}

bool SandboxApp::hasInputFlag(uint32_t flag) const {
  return (_inputFlags & flag) != 0;
}
