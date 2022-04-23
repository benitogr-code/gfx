#include "scene_playground.h"

#include "../utils/mesh_utils.h"
#include <imgui.h>

void ScenePlayground::init() {
  auto floorMaterial = getAssetManager().getMaterial("stone_floor");

  _boxes[0].attachModel(getAssetManager().loadModel("models/wooden_crate.gfx"));
  _boxes[0].setPosition(glm::vec3(3.0f, 1.0f, 0.5f));
  _boxes[0].setRotation(glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
  _boxes[0].setFlag(Entity::Flags::RenderShadow, true);

  _boxes[1].attachModel(getAssetManager().loadModel("models/wooden_crate.gfx"));
  _boxes[1].setPosition(glm::vec3(-3.0f, 1.0f, 0.5f));
  _boxes[1].setFlag(Entity::Flags::RenderShadow, true);

  _ground.attachModel(GfxModel::Create(MeshUtils::CreateGroundPlane(2.0f, 15, 2.0f), floorMaterial));
  //_ground.setFlag(Entity::Flags::RenderShadow, true);

  _cyborg.attachModel(getAssetManager().loadModel("models/cyborg.gfx"));
  _cyborg.setPosition(glm::vec3(0.0f, 0.2f, -0.7f));
  _cyborg.setFlag(Entity::Flags::RenderShadow, true);

  ColorRGB    colors[2] = { ColorRGB(0.20f, 0.80f, 0.63f), ColorRGB(0.5f, 0.2f, 0.1f) };
  const char* labels[2] = { "Light 1", "Light 2" };

  for (int i = 0; i < 2; ++i) {
    _pointLights[i].setName(labels[i]);
    _pointLights[i].setFlag(Entity::Flags::DisplayName, true);
    _pointLights[i].setFlag(Entity::Flags::Hidden, true);
    _pointLights[i].attachModel(getAssetManager().loadModel("models/point_light.gfx"));
    _pointLights[i].cloneModelMaterial();
    _pointLights[i].getModelMaterial()->setParamVec3("material.color", colors[i]);
    _pointLights[i].setScale(glm::vec3(0.1f));

    Light::Properties props;
    props.color = colors[i];
    props.ambientMultiplier = 0.1f;
    props.specularMultiplier = 1.2f;
    props.attenuationConstant = 0.01f;
    props.attenuationLinear = 0.1f;
    props.attenuationQuadratic = 0.05f;
    _pointLights[i].attachLight(props);
  }
}

void ScenePlayground::update(float frameTime) {
  _time += frameTime;

  _pointLights[0].setPosition(glm::vec3(sinf(_time)*4.0f, 4.5f, cosf(_time)*4.0f));
  _pointLights[1].setPosition(glm::vec3(-sinf(_time)*3.5f, 5.0f, cosf(_time)*3.5f));
  _cyborg.setRotation(glm::angleAxis(_time*0.7f, glm::vec3(0.0f, 1.0f, 0.0f)));

  _sunPosition = computeSunPosition();
}

void ScenePlayground::render(Renderer& renderer) {
  auto& mainLight = renderer.getMainLight();
  mainLight.position = _sunPosition;
  mainLight.properties.color = _sunColor;
  mainLight.properties.ambientMultiplier = _sunAmbientMult;
  mainLight.properties.specularMultiplier = _sunSpecularMult;

  for (int i = 0; i < 2 ; i++) {
    _pointLights[i].render(renderer);
  }

  for (int i = 0; i < 2 ; i++) {
    _boxes[i].render(renderer);
  }

  _ground.render(renderer);
  _cyborg.render(renderer);
}

void ScenePlayground::onInputEvent(const InputEvent& event) {
  const bool pressed = (event.state == InputState_Pressed);

  if (pressed && event.key == KeyboardKey_1) {
    const bool hide = _pointLights[0].hasFlag(Entity::Flags::Hidden);
    _pointLights[0].setFlag(Entity::Flags::Hidden, !hide);
  }
  if (pressed && event.key == KeyboardKey_2) {
    const bool hide = _pointLights[1].hasFlag(Entity::Flags::Hidden);
    _pointLights[1].setFlag(Entity::Flags::Hidden, !hide);
  }
}

void ScenePlayground::onGUI() {
  ImGui::Separator();
  ImGui::Text("Playground Scene");
  ImGui::Separator();

  if (ImGui::CollapsingHeader("Sun light", ImGuiTreeNodeFlags_DefaultOpen)){
    ImGui::ColorEdit3("Color", glm::value_ptr(_sunColor));
    ImGui::SliderFloat("Ambient", &_sunAmbientMult, 0.05f, 0.4f);
    ImGui::SliderFloat("Specular", &_sunSpecularMult, 0.3f, 1.5f);
    ImGui::SliderFloat("Longitude", &_sunLongitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Latitude", &_sunLatitude, -5.0f, 85.0f);
  }

  if (ImGui::CollapsingHeader("Point lights", ImGuiTreeNodeFlags_DefaultOpen)){
    for (int i = 0; i < 2; ++i) {
      const bool off = _pointLights[i].hasFlag(Entity::Flags::Hidden);
      ImGui::Text("Light %d [%s] (Toggle with key '%d')", i+1, off ? "OFF" : "ON", i+1);
    }
  }
}

glm::vec3 ScenePlayground::computeSunPosition() const {
  const float latitude = glm::radians(_sunLatitude);
  const float longitude = glm::radians(_sunLongitude);

  glm::vec3 pos;
  pos.x = 1000.0f * cosf(latitude) * cosf(longitude);
  pos.y = 1000.0f * sinf(latitude);
  pos.z = 1000.0f * cosf(latitude) * sinf(longitude);

  return pos;
}
