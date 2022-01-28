#pragma once

#include "scene.h"
#include "../utils/entity.h"


class SceneCubemaps: public Scene {
private:
  struct RefractionIndex {
    std::string name;
    float       value;
  };

public:
  SceneCubemaps(AssetManager& manager)
    : Scene(manager) {
      _time = 0.0f;
  }

  virtual void init() override;
  virtual void update(float frameTime) override;
  virtual void render(Renderer& renderer) override;
  virtual void onInputEvent(const InputEvent& event) override;
  virtual void onGUI() override;

private:
  Entity    _skybox;
  Entity    _sphere;
  Entity    _box;
  float     _time;

  std::vector<RefractionIndex> _refractionIndices;
  size_t _currentIndex;
};