#include "sandbox_app.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _bgColor(0.2f, 0.2f, 0.2f) {

}

bool SandboxApp::onInit() {
  MeshCreateParams meshParams;
  meshParams.vertices.reserve(4);
  meshParams.indices.reserve(6);

  meshParams.vertices.push_back(Vertex({ glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(), glm::vec2() }));
  meshParams.vertices.push_back(Vertex({ glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(), glm::vec2() }));
  meshParams.vertices.push_back(Vertex({ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(), glm::vec2() }));
  meshParams.vertices.push_back(Vertex({ glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(), glm::vec2() }));

  meshParams.indices = {
    0, 1, 3,
    1, 2, 3
  };

  _mesh = Mesh::Create(meshParams);

  _model = Model3D::Create("objects/planet/planet.obj");

  ShaderCreateParams shaderParams;
  shaderParams.name = "test";
  shaderParams.vertexShaderPath = "shaders/test.vert";
  shaderParams.fragmentShaderPath = "shaders/test.frag";

  _shader = Shader::Create(shaderParams);
  _shader->use();

  _camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), 1.0f);

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  LOG_WARN("Input event: Key={0} State={1}", event.keyId, event.state);
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  getRenderer()->setClearColor(_bgColor);

  _shader->setUniformMatrix4("uViewProjection", _camera.getViewProjection());
  //getRenderer()->draw(_mesh, _shader);
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
