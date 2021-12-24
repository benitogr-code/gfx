#include "camera_orthographic.h"

CameraOrthographic::CameraOrthographic()
  : CameraOrthographic(0.0f, 800.0f, 0.0f, 600.0f) {
}

CameraOrthographic::CameraOrthographic(float left, float right, float bottom, float top) {
  _projectionMatrix = glm::ortho(left, right, bottom, top);
  _viewMatrix = glm::identity<glm::mat4x4>();
  _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
  _position = glm::vec2(0.0f);
}

void CameraOrthographic::setPosition(const glm::vec2& position) {
  _position = position;
  updateViewMatrices();
}

void CameraOrthographic::updateViewMatrices() {
  const auto transform = glm::translate(glm::mat4x4(1.0f), glm::vec3(_position, 0.0f));

  _viewMatrix = glm::inverse(transform);
  _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}
