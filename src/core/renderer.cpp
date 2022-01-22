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

  _mainLight.position = glm::vec3(20.0f, 30.0f, 100.0f);
  _mainLight.properties.color = ColorRGB(1.0f);
  _mainLight.properties.ambientMultiplier = 0.2f;
  _mainLight.properties.specularMultiplier = 0.85f;

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

void Renderer::drawLight(const Light& light) {
  if (_lightsList.size() < MaxPointLights) {
    _lightsList.push_back(light);
  }
}

void Renderer::drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& worldTM) {
  RenderItem item;
  item.mesh = mesh;
  item.material = material;
  item.modelTM = worldTM;

  _renderList.push_back(item);
}

void Renderer::beginFrame() {
  glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  _renderList.clear();
  _lightsList.clear();
}

void Renderer::endFrame() {
  // Prepare UBOs
  _uboCamera->writeBegin();
  _uboCamera->writeVec3(_viewCamera.getPosition());
  _uboCamera->writeMat4(_viewCamera.getView());
  _uboCamera->writeMat4(_viewCamera.getViewProjection());
  _uboCamera->writeEnd();

  _uboLights->writeBegin();
  _uboLights->writeVec3(-glm::normalize(_mainLight.position));
  _uboLights->writeVec3(_mainLight.properties.color * _mainLight.properties.ambientMultiplier);
  _uboLights->writeVec3(_mainLight.properties.color);
  _uboLights->writeVec3(_mainLight.properties.color * _mainLight.properties.specularMultiplier);

  _uboLights->writeInt((int)_lightsList.size());
  for (auto& light : _lightsList) {
    _uboLights->writeVec3(light.position);
    _uboLights->writeVec3(light.properties.color * light.properties.ambientMultiplier);
    _uboLights->writeVec3(light.properties.color);
    _uboLights->writeVec3(light.properties.color * light.properties.specularMultiplier);
    _uboLights->writeFloat(light.properties.attenuationConstant);
    _uboLights->writeFloat(light.properties.attenuationLinear);
    _uboLights->writeFloat(light.properties.attenuationQuadratic);
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
