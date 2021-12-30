#pragma once

#include "system/application.h"
#include "system/graphics/buffers.h"
#include "system/graphics/shader.h"

class SandboxApp: public Application {
public:
  SandboxApp();

protected:
  // Application
  virtual bool onInit() override;
  virtual void onShutdown() override;
  virtual void onInputEvent(const InputEvent& event) override;
  virtual void onUpdate(const UpdateContext& ctx) override;
  virtual void onGUI() override;

private:
  VAORef _vao;
  ShaderRef      _shader;
  ColorRGB       _drawColor;
  float          _offset;
};
