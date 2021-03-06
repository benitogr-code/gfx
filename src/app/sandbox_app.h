#pragma once

#include "core/application.h"
#include "demos/scene.h"

class SandboxApp: public Application {
private:
  struct CameraSettings {
    CameraSettings()
      : position(0.0f, 5.0f, 10.0f)
      , yaw(0.0f)
      , pitch(0.0f)
      , movementSpeed(7.5f)
      , mouseSensitivity(0.2f)
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
      const auto xQuat = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
      const auto yQuat = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

      return yQuat * xQuat;
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
    InputFlag_MoveForward  = BIT(0),
    InputFlag_MoveBackward = BIT(1),
    InputFlag_MoveLeft     = BIT(2),
    InputFlag_MoveRight    = BIT(3),
    InputFlag_MouseLeft    = BIT(4),
    InputFlag_MouseRight   = BIT(5),
  };

public:
  SandboxApp();

protected:
  // Application
  virtual bool onInit() override;
  virtual void onShutdown() override;
  virtual void onInputEvent(const InputEvent& event) override;
  virtual void onMouseEvent(const MouseEvent& event) override;
  virtual void onUpdate(const UpdateContext& ctx) override;
  virtual void onGUI() override;

private:
  void changeScene(uint32_t id);
  void setInputFlag(uint32_t flag, bool set);
  bool hasInputFlag(uint32_t flag) const;

private:
  CameraSettings _camera;
  uint32_t       _inputFlags;
  glm::ivec2     _mousePosition;

  std::unique_ptr<Scene> _scene;
  uint32_t               _selectedScene;
};
