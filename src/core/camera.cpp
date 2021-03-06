#include "camera.h"

Camera::Camera()
  : Camera(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f) {
}

Camera::Camera(const glm::vec3& position, float aspectRatio, float fov, float nearPlane, float farPlane) {
  _aspectRatio = aspectRatio;
  _fov = glm::radians(fov);
  _nearPlane = nearPlane;
  _farPlane = farPlane;

  _world = glm::translate(glm::mat4(1.0f), position);
  _view = glm::inverse(_world);
  _projection = glm::perspective(_fov, aspectRatio, nearPlane, farPlane);
  _viewProjection = _projection * _view;
}

void Camera::setFov(float fov) {
  const float fovRad = glm::radians(fov);

  if (fabs(fovRad - _fov) < FLT_EPSILON)
    return;

  _fov = fovRad;
  _projection = glm::perspective(_fov, _aspectRatio, _nearPlane, _farPlane);
  _viewProjection = _projection * _view;
}

void Camera::setAspectRatio(float aspectRatio) {
  if (fabs(aspectRatio - _aspectRatio) < FLT_EPSILON)
    return;

  _aspectRatio = aspectRatio;
  _projection = glm::perspective(_fov, _aspectRatio, _nearPlane, _farPlane);
  _viewProjection = _projection * _view;
}

void Camera::setWorldLocation(const glm::vec3& position, const glm::quat& rotation) {
  _world = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation);
  _view = glm::inverse(_world);
  _viewProjection = _projection * _view;
}

void Camera::setViewport(int width, int height) {
  _viewPort.x = width;
  _viewPort.y = height;
}

glm::vec3 Camera::worldToScreenCoordinates(const glm::vec3& position) const {
  return glm::project(position, _view, _projection, glm::vec4(0, 0, _viewPort.x, _viewPort.y));
}
