#pragma once

#include "scene.h"
#include "../utils/entity.h"


class SceneCubemaps: public Scene {
public:
  SceneCubemaps(AssetManager& manager)
    : Scene(manager) {
  }

  virtual void init() override;
  virtual void update(float frameTime) override;
  virtual void render(Renderer& renderer) override;
  virtual void onInputEvent(const InputEvent& event) override;
  virtual void onGUI() override;

private:
  Entity    _skybox;
};