#pragma once

#include "buffers.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
  glm::vec3 tangent;
};

class Mesh;
typedef std::shared_ptr<Mesh> MeshRef;

struct MeshCreateParams {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
};

class Mesh {
public:
  void draw();

  static MeshRef Create(const MeshCreateParams& params);

private:
  Mesh() = delete;
  Mesh(const Mesh& mesh) = delete;

  Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

  void setup();

private:
  std::vector<Vertex>       _vertices;
  std::vector<unsigned int> _indices;

  VAORef _vao;
};
