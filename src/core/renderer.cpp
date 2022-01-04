#include "renderer.h"
#include "graphics/debug_utils.h"

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>

#define UBO_CAMERA_IDX 0

Renderer::Renderer()
    : _clearColor(0.0f)
    , _wireframeEnabled(false) {

    _viewCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), 1.0f);
}

void Renderer::init() {
    _uboCamera = UBO::Create(
        UBO_CAMERA_IDX,
        BufferLayout({
            { BufferItemType::Float3, "view_pos" },
            { BufferItemType::Mat4,   "mtx_view" },
            { BufferItemType::Mat4,   "mtx_viewProj"}
        })
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
}


void Renderer::toggleWireframe() {
    _wireframeEnabled = !_wireframeEnabled;
    glPolygonMode(GL_FRONT_AND_BACK, _wireframeEnabled ? GL_LINE : GL_FILL);
}

void Renderer::draw(MeshRef& mesh, ShaderRef& shader) {
  mesh->draw(shader);
}

void Renderer::draw(Model3DRef& model, ShaderRef& shader) {
  model->draw(shader);
}

void Renderer::beginFrame() {
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Renderer::endFrame() {
    // Prepare UBOs
    _uboCamera->writeBegin();
    _uboCamera->writeVec3(_viewCamera.getPosition());
    _uboCamera->writeMat4(_viewCamera.getView());
    _uboCamera->writeMat4(_viewCamera.getViewProjection());
    _uboCamera->writeEnd();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GL_CHECK_ERROR();
}