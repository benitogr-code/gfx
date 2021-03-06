#pragma once

class Camera {
  public:
    Camera();
    Camera(const glm::vec3& position, float aspectRatio, float fov = 65.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

    const glm::vec3  getPosition() const { return glm::vec3(_world[3]); }
    const glm::mat4& getView() const { return _view; }
    const glm::mat4& getProjection() const { return _projection; }
    const glm::mat4& getViewProjection() const { return _viewProjection; }
    const float   getFov() const { return _fov; }
    const glm::vec2& getViewport() const { return _viewPort; }

    void setFov(float fov);
    void setAspectRatio(float aspectRatio);
    void setWorldLocation(const glm::vec3& position, const glm::quat& rotation);
    void setViewport(int width, int height);

    glm::vec3 worldToScreenCoordinates(const glm::vec3& position) const;

  private:
    glm::mat4 _world;
    glm::mat4 _view;
    glm::mat4 _projection;
    glm::mat4 _viewProjection;
    glm::vec2 _viewPort;

    float _fov;
    float _aspectRatio;
    float _nearPlane;
    float _farPlane;
};
