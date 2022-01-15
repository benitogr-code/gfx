#pragma once

#include "input.h"
#include "renderer.h"
#include "asset_manager.h"
#include "window.h"

struct UpdateContext {
  UpdateContext(float _frameTime, uint32_t _frameId)
    : frameTime(_frameTime)
    , frameId(_frameId) {
  }

  float    frameTime;
  uint32_t frameId;
};

class Application {
public:
  Application();
  virtual ~Application();

  bool init(const WindowDesc& desc);
  void shutdown();
  void run();

protected:
  Window*   getWindow() const { return _window.get(); };
  Renderer* getRenderer() const { return _renderer.get(); }
  AssetManager* getAssetManager() const { return _assetManager.get(); }

  // To be implemented by custom application
  virtual bool onInit() = 0;
  virtual void onShutdown() = 0;
  virtual void onInputEvent(const InputEvent& event) = 0;
  virtual void onMouseEvent(const MouseEvent& event) = 0;
  virtual void onUpdate(const UpdateContext& ctx) = 0;
  virtual void onGUI() = 0;

private:
  void checkSystemEvents();

private:
  std::unique_ptr<Window> _window;
  std::unique_ptr<Input> _input;
  std::unique_ptr<Renderer> _renderer;
  std::unique_ptr<AssetManager> _assetManager;

  bool     _running;
};

typedef std::function<Application* ()> ApplicationCreator;

struct StartupParams {
  ApplicationCreator appCreatorFunc;
  WindowDesc  windowDesc;
  std::string assetsFolder;
};

int startApplication(const StartupParams& params);
