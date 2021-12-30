#pragma once

class Camera {
  public:
    Camera();
    Camera(const glm::vec3& position, float aspectRatio, float fov = 65.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);

    const glm::vec3  getPosition() const { return glm::vec3(_world[3]); }
    const glm::mat4& getViewMatrix() const { return _view; }
    const glm::mat4& getProjectionMatrix() const { return _projection; }
    const float   getFov() const { return _fov; }

    void setFov(float fov);
    void setAspectRatio(float aspectRatio);

  private:
    glm::mat4 _world;
    glm::mat4 _view;
    glm::mat4 _projection;
    glm::mat4 _viewProjection;

    float _fov;
    float _aspectRatio;
    float _nearPlane;
    float _farPlane;
};
