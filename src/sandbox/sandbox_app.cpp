#include "sandbox_app.h"
#include "utils/mesh_utils.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _bgColor(0.2f, 0.2f, 0.2f) {

}

bool SandboxApp::onInit() {
  _mesh = MeshUtils::CreateCube(1.75f);
  _model = Model3D::Create("objects/planet/planet.obj");

  ShaderCreateParams shaderParams;
  shaderParams.name = "test";
  shaderParams.vertexShaderPath = "shaders/test.vert";
  shaderParams.fragmentShaderPath = "shaders/test.frag";

  _shader = Shader::Create(shaderParams);
  _shader->use();

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  LOG_WARN("Input event: Key={0} State={1}", event.keyId, event.state);
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  getRenderer()->setClearColor(_bgColor);

  const glm::vec3 axis(0.0f, 1.0f, -1.0f);
  const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::normalize(axis));
  const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.0f));

  _shader->setUniformMatrix4("uViewProjection", getRenderer()->getViewCamera().getViewProjection());

  _shader->setUniformMatrix4("uModel", rotation * translation);
  getRenderer()->draw(_mesh, _shader);

  _shader->setUniformMatrix4("uModel", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.6f)), glm::vec3(2.0f, 0.0f, 0.0f)));
  getRenderer()->draw(_model, _shader);
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

    ImGui::Spacing();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}
