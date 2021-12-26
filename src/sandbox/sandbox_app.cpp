#include "sandbox_app.h"
#include <imgui.h>
#include <glad/glad.h>

SandboxApp::SandboxApp()
  : _bgColor(0.0f) {

}

bool SandboxApp::onInit() {
  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  LOG_WARN("Input event: Key={0} State={1}", event.keyId, event.state);
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  glClearColor(_bgColor.r, _bgColor.g, _bgColor.b, _bgColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void SandboxApp::onGUI() {
  static int counter = 0;

  ImGui::Begin("Demo", nullptr, ImGuiWindowFlags_MenuBar|ImGuiWindowFlags_NoMove);
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Scenes")) {
        if (ImGui::MenuItem("Multi-texture", "Ctrl+1")) {
          LOG_INFO("Multi-texture scene selected");
        }
        if (ImGui::MenuItem("Shadows", "Ctrl+2")) {
          LOG_INFO("Shadows scene selected");
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    ImGui::Spacing();
    ImGui::Text("Select your background");
    ImGui::ColorEdit3("Color", glm::value_ptr(_bgColor));

    if (ImGui::Button("Button"))
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}
