#pragma once

#include "system/application.h"

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
  glm::vec4 _bgColor;
};
