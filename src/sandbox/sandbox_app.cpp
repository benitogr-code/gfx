#include "sandbox_app.h"

#include <imgui.h>

SandboxApp::SandboxApp()
  : _drawColor(0.5f, 0.0f, 0.0f) {

}

bool SandboxApp::onInit() {
  float vertices[] = {
     0.5f,  0.5f, 0.0f, // top right
     0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
  };
  unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
  };

  auto vbo = VertexBuffer::Create(
    vertices,
    sizeof(vertices),
    BufferLayout({
      { BufferItemType::Float3, "position" }
    })
  );
  auto ibo = IndexBuffer::Create(indices, 6);

  _vao = VertexArray::Create();
  _vao->addVertextBuffer(vbo);
  _vao->setIndexBuffer(ibo);

  ShaderCreateParams shaderParams;
  shaderParams.name = "test";
  shaderParams.vertexShaderPath = "shaders/test.vs";
  shaderParams.fragmentShaderPath = "shaders/test.fs";

  _shader = Shader::Create(shaderParams);

  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  LOG_WARN("Input event: Key={0} State={1}", event.keyId, event.state);
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
  getRenderer()->setClearColor(ColorRGB(0.2f, 0.2f, 0.2f));

  _shader->setUniformVec3("uColor", _drawColor);
  getRenderer()->drawVertexArray(_vao, _shader);
}

void SandboxApp::onGUI() {
  static int counter = 0;

  ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
  ImGui::Begin("Settings", nullptr);
    ImGui::ColorEdit3("Background color", glm::value_ptr(_drawColor));

    if (ImGui::Button("Toggle wireframe")) {
      getRenderer()->toggleWireframe();
    }

    ImGui::Spacing();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}
