#pragma once

#include "core/asset_manager.h"
#include "core/input.h"
#include "core/renderer.h"

class Scene {
public:
  Scene(AssetManager& manager)
    : _assetManager(manager) {
  }
  virtual ~Scene() {};

  virtual void init() = 0;
  virtual void update(float frameTime) = 0;
  virtual void render(Renderer& renderer) = 0;
  virtual void onInputEvent(const InputEvent& event) = 0;
  virtual void onGUI() = 0;

protected:
  AssetManager& getAssetManager() { return _assetManager; }

private:
  AssetManager& _assetManager;
};