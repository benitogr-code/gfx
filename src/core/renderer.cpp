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
      {
        UBO::newVec(3),             // position
        UBO::newColumnMatrix(4, 4), // view
        UBO::newColumnMatrix(4, 4), // view-projection
      }
  );

  _uboLights = UBO::Create(
    UBO_LIGHTS_IDX,
      {
      UBO::newStruct({  // Main Light
        UBO::newVec(3), // - direction
        UBO::newVec(3), // - ambient
        UBO::newVec(3), // - diffuse
        UBO::newVec(3)  // - specular
      }),
      UBO::newScalar(), // Number of point lights
      UBO::newArray(MaxPointLights, UBO::newStruct({  // Point Lights Array
          UBO::newVec(3), // - position
          UBO::newVec(3), // - ambient
          UBO::newVec(3), // - diffuse
          UBO::newVec(3), // - specular
          UBO::newScalar(), // - attenuation constant
          UBO::newScalar(), // - attenuation linear
          UBO::newScalar(), // - attenuation quadratic
        })
      )
      }
  );

  _mainLight.direction = glm::normalize(glm::vec3(-0.2f, -0.3f, -1.0f));
  _mainLight.ambient = ColorRGB(0.2f);
  _mainLight.diffuse = ColorRGB(1.0f);
  _mainLight.specular = ColorRGB(0.85f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
}


void Renderer::toggleWireframe() {
  _wireframeEnabled = !_wireframeEnabled;
  if (_wireframeEnabled) {
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void Renderer::drawLight(const PointLight& light) {
  if (_pointLights.size() < MaxPointLights) {
    _pointLights.push_back(light);
  }
}

void Renderer::drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& modelMtx) {
  RenderItem item;
  item.mesh = mesh;
  item.material = material;
  item.modelTM = modelMtx;

  _renderList.push_back(item);
}

void Renderer::beginFrame() {
  glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  _renderList.clear();
  _pointLights.clear();
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

  _uboLights->writeInt((int)_pointLights.size());
  for (auto& light : _pointLights) {
    _uboLights->writeVec3(light.position);
    _uboLights->writeVec3(light.ambient);
    _uboLights->writeVec3(light.diffuse);
    _uboLights->writeVec3(light.specular);
    _uboLights->writeFloat(light.attConstant);
    _uboLights->writeFloat(light.attLinear);
    _uboLights->writeFloat(light.attQuadratic);
  }
  _uboLights->writeEnd();

  // Go through render list
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
