#include "renderer.h"

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>

Renderer::Renderer()
    : _clearColor(0.0f)
    , _wireframeEnabled(false) {
}

void Renderer::toggleWireframe() {
    _wireframeEnabled = !_wireframeEnabled;
    glPolygonMode(GL_FRONT_AND_BACK, _wireframeEnabled ? GL_LINE : GL_FILL);
}

void Renderer::drawVertexArray(const VertexArrayRef& va, const ShaderRef& shader) {
  shader->use();
  va->bind();
  glDrawElements(GL_TRIANGLES, va->indexCount(), GL_UNSIGNED_INT, 0);
}

void Renderer::beginFrame() {
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Renderer::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}