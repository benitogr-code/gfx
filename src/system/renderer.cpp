#include "renderer.h"

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>


struct GLParam {
    const char* name;
    GLenum      param;
};

constexpr std::array<GLParam, 9> GL_PARAMS = {{
    { "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS },
    { "GL_MAX_CUBE_MAP_TEXTURE_SIZE", GL_MAX_CUBE_MAP_TEXTURE_SIZE },
    { "GL_MAX_DRAW_BUFFERS", GL_MAX_DRAW_BUFFERS },
    { "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", GL_MAX_FRAGMENT_UNIFORM_COMPONENTS },
    { "GL_MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS },
    { "GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE },
    { "GL_MAX_VERTEX_ATTRIBS", GL_MAX_VERTEX_ATTRIBS },
    { "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS },
    { "GL_MAX_VERTEX_UNIFORM_COMPONENTS", GL_MAX_VERTEX_UNIFORM_COMPONENTS }
}};

Renderer::Renderer()
    : _clearColor(0.0f)
    , _wireframeEnabled(false) {
}

void Renderer::init() {
    LOG_INFO("OpenGL capabilities:");

    for (size_t i = 0; i < GL_PARAMS.size(); ++i) {
        int value = 0;
        glGetIntegerv(GL_PARAMS[i].param, &value);
        LOG_INFO("\t{0} {1}", GL_PARAMS[i].name, value);
    }

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
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}