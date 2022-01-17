#include "renderer.h"
#include "graphics/debug_utils.h"

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>

#define UBO_CAMERA_IDX 0
#define UBO_LIGHTS_IDX 1

Renderer::Renderer()
  : _clearColor(0.0f)
  , _wireframeEnabled(false) {
    _viewCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), 1.0f);
    _renderList.reserve(256);
}

void Renderer::init() {
  _uboCamera = UBO::Create(
      UBO_CAMERA_IDX,
      BufferLayout({
          { BufferItemType::Float3, "position" },
          { BufferItemType::Mat4,   "view" },
          { BufferItemType::Mat4,   "viewProj"}
      })
  );
  _uboLights = UBO::Create(
    UBO_LIGHTS_IDX,
      BufferLayout({
          { BufferItemType::Float3, "direction" },
          { BufferItemType::Float3, "ambient" },
          { BufferItemType::Float3, "diffuse"},
          { BufferItemType::Float3, "specular"},
      })
  );

  _mainLight.direction = glm::normalize(glm::vec3(-0.2f, -0.3f, -1.0f));
  _mainLight.ambient = ColorRGB(0.2f);
  _mainLight.diffuse = ColorRGB(1.0f);
  _mainLight.specular = ColorRGB(0.85f);

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

void Renderer::draw(const RenderItem& item) {
  _renderList.push_back(item);
}

void Renderer::beginFrame() {
  glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  _renderList.clear();
}

void Renderer::endFrame() {
  // Prepare UBOs
  _uboCamera->writeBegin();
  _uboCamera->writeVec3(_viewCamera.getPosition());
  _uboCamera->writeMat4(_viewCamera.getView());
  _uboCamera->writeMat4(_viewCamera.getViewProjection());
  _uboCamera->writeEnd();

  _uboLights->writeBegin();
  _uboLights->writeVec3(_mainLight.direction);
  _uboLights->writeVec3(_mainLight.ambient);
  _uboLights->writeVec3(_mainLight.diffuse);
  _uboLights->writeVec3(_mainLight.specular);
  _uboLights->writeEnd();

  for (auto item : _renderList) {
    auto shader = item.material->getShader();
    shader->use();
    shader->setUniformBlockBind("Camera", UBO_CAMERA_IDX);
    shader->setUniformBlockBind("Lights", UBO_LIGHTS_IDX);
    shader->setUniformMatrix4("mtx_model", item.modelTM);
    item.material->apply();
    item.mesh->draw();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  GL_CHECK_ERROR();
}
