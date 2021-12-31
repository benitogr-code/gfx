#pragma once

#include "system/application.h"
#include "system/graphics/camera.h"
#include "system/graphics/mesh.h"
#include "system/graphics/model3d.h"
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
  ShaderRef      _shader;
  MeshRef        _mesh;
  Model3DRef     _model;
  ColorRGB       _bgColor;
};
