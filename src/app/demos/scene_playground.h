#pragma once

#include "scene.h"
#include "../utils/entity.h"


class ScenePlayground: public Scene {
public:
  ScenePlayground(AssetManager& manager)
    : Scene(manager) {
    _time = 0.0f;
    _sunLatitude = 30.0f;
    _sunLongitude = 70.0f;
    _sunColor = ColorRGB(0.6f);
    _sunAmbientMult = 0.2f;
    _sunSpecularMult = 0.8f;
    _sunPosition = computeSunPosition();
  }

  virtual void init() override;
  virtual void update(float frameTime) override;
  virtual void render(Renderer& renderer) override;
  virtual void onInputEvent(const InputEvent& event) override;
  virtual void onGUI() override;

private:
  glm::vec3 computeSunPosition() const;

private:
  Entity    _ground;
  Entity    _boxes[2];
  Entity    _cyborg;
  Entity    _pointLights[2];

  float     _time;
  glm::vec3 _sunPosition;
  ColorRGB  _sunColor;
  float     _sunAmbientMult;
  float     _sunSpecularMult;
  float     _sunLongitude;
  float     _sunLatitude;
};