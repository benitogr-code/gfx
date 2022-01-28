#include "scene_cubemaps.h"

#include "../utils/mesh_utils.h"
#include <imgui.h>

void SceneCubemaps::init() {
  // Init refraction indices
  _refractionIndices.reserve(4);
  _refractionIndices.push_back({ "Water", 1.33f });
  _refractionIndices.push_back({ "Ice", 1.309f });
  _refractionIndices.push_back({ "Glass", 1.52f });
  _refractionIndices.push_back({ "Diamond", 2.42 });
  _currentIndex = 0;

  // Cubemap texture
  const char* faces[Texture3DCreateParams::Faces::Count] = {
    "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
  };

  Texture3DCreateParams params;
  for (int i = 0; i < Texture3DCreateParams::Faces::Count; ++i) {
    params.filePaths[i] = std::string("materials/textures/skybox/").append(faces[i]);
  }

  // Skybox
  TextureRef skyboxTexture = Texture::CreateCubemap(params);
  MaterialRef skyboxMaterial = Material::Create(getAssetManager().getShader("skybox"));
  skyboxMaterial->setTextureSlot(MaterialSlotId_0, "material.cubemap_skybox", skyboxTexture);

  _skybox.attachModel(GfxModel::Create(MeshUtils::CreateSkybox(), skyboxMaterial));

  // Reflective Sphere
  MaterialRef sphereMaterial = Material::Create(getAssetManager().getShader("env_mapping"));
  sphereMaterial->setTextureSlot(MaterialSlotId_0, "material.cubemap", skyboxTexture);
  sphereMaterial->setParamInt("material.refract", 0);

  _sphere.attachModel(getAssetManager().loadModel("models/sphere.gfx"));
  _sphere.setOverrideMaterial(sphereMaterial);
  _sphere.setPosition(glm::vec3(-2.0f, 1.5f, 0.0f));
  _sphere.setScale(glm::vec3(1.3f));

  // Refractive Box
  MaterialRef boxMaterial = Material::Create(getAssetManager().getShader("env_mapping"));
  boxMaterial->setTextureSlot(MaterialSlotId_0, "material.cubemap", skyboxTexture);
  boxMaterial->setParamInt("material.refract", 1);
  boxMaterial->setParamFloat("material.refract_index", _refractionIndices[_currentIndex].value);

  _box.attachModel(getAssetManager().loadModel("models/wooden_crate.gfx"));
  _box.setOverrideMaterial(boxMaterial);
  _box.setPosition(glm::vec3(2.0f, 1.5f, 0.0f));
}

void SceneCubemaps::update(float frameTime) {
  _time += frameTime;

  _box.getModelMaterial()->setParamFloat("material.refract_index", _refractionIndices[_currentIndex].value);

  _sphere.setPosition(glm::vec3(-2.0f, 1.5f + sinf(_time), 0.0f));
  _box.setRotation(glm::angleAxis(_time*0.3f, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))));
}

void SceneCubemaps::render(Renderer& renderer) {
  _sphere.render(renderer);
  _box.render(renderer);
  _skybox.render(renderer);
}

void SceneCubemaps::onInputEvent(const InputEvent& event) {

}

void SceneCubemaps::onGUI() {
  ImGui::Separator();
  ImGui::Text("Cubemaps Scene");
  ImGui::Separator();

  if (ImGui::BeginCombo("Refraction Index", _refractionIndices[_currentIndex].name.c_str())) {
    for (size_t i = 0; i < _refractionIndices.size(); i++) {
      bool selected = (_currentIndex == i);
      if (ImGui::Selectable(_refractionIndices[i].name.c_str(), selected)) {
         _currentIndex = i;
      }
      if (selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
}
