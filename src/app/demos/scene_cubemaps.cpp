#include "scene_cubemaps.h"

#include "../utils/mesh_utils.h"
#include <imgui.h>

void SceneCubemaps::init() {
  auto skyboxMaterial = getAssetManager().getMaterial("skybox");

  _skybox.attachModel(GfxModel::Create(MeshUtils::CreateSkybox(), skyboxMaterial));
}

void SceneCubemaps::update(float frameTime) {

}

void SceneCubemaps::render(Renderer& renderer) {
  _skybox.render(renderer);
}

void SceneCubemaps::onInputEvent(const InputEvent& event) {

}

void SceneCubemaps::onGUI() {
  ImGui::Separator();
  ImGui::Text("Cubemaps Scene");
  ImGui::Separator();
}
