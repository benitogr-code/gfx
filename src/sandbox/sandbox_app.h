#pragma once

#include "system/application.h"
#include "system/graphics/camera.h"
#include "system/graphics/mesh.h"
#include "system/graphics/model3d.h"
#include "system/graphics/shader.h"

class SandboxApp: public Application {
private:
  struct CameraSettings {
    CameraSettings()
      : position(0.0f, 0.0f, 10.0f)
      , yaw(0.0f)
      , pitch(0.0f)
      , movementSpeed(3.0f)
      , mouseSensitivity(0.1f)
      , fov(60.0f) {
        updateAxis();
    }

    void updateAxis() {
      const glm::mat4 camRotation = glm::toMat4(getQuat());

      forward = glm::vec3(camRotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
      right = glm::vec3(camRotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
      up  = glm::vec3(camRotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    }

    glm::quat getQuat() const {
      return glm::quat(yaw, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::quat(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float fov;
  };

  enum {
    InputFlag_MoveForward  = 0x00000001,
    InputFlag_MoveBackward = 0x00000002,
    InputFlag_MoveLeft     = 0x00000004,
    InputFlag_MoveRight    = 0x00000008,
    InputFlag_MouseLeft    = 0x00000010,
    InputFlag_MouseRight   = 0x00000020,
  };

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
  void setInputFlag(uint32_t flag, bool set);
  bool hasInputFlag(uint32_t flag) const;

private:
  CameraSettings _camera;
  uint32_t       _inputFlags;
  glm::vec2      _mousePosition;

  ShaderRef      _shader;
  MeshRef        _mesh;
  Model3DRef     _model;
  ColorRGB       _bgColor;
};
