#pragma once

class CameraOrthographic {
public:
  CameraOrthographic();
  CameraOrthographic(float left, float right, float bottom, float top);

  void setPosition(const glm::vec2& position);

  const glm::vec2& getPosition() const {
    return _position;
  }
  const glm::mat4x4& getViewMatrix() const {
    return _viewMatrix;
  }
  const glm::mat4x4& getProjectionMatrix() const {
    return _projectionMatrix;
  }
  const glm::mat4x4& getViewProjectionMatrix() const {
    return _viewProjectionMatrix;
  }

private:
  void updateViewMatrices();

private:
  glm::mat4x4 _viewMatrix;
  glm::mat4x4 _projectionMatrix;
  glm::mat4x4 _viewProjectionMatrix;

  glm::vec2 _position;
};
