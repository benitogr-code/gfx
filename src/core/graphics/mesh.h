#pragma once

#include "buffers.h"
#include "shader.h"
#include "texture.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct TextureSlot {
    TextureRef  texture;
    std::string slot;
};

struct MeshMaterial {
    MeshMaterial()
        : color(1.0f)
        , ambientFactor(0.1f)
        , specularFactor(0.5f) {

    }

    glm::vec3 color;
    float     ambientFactor;
    float     specularFactor;
};

class Mesh;
typedef std::shared_ptr<Mesh> MeshRef;

struct MeshCreateParams {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureSlot> textures;
};

class Mesh {
public:
    void setMaterial(const MeshMaterial& material) { _material = material; }
    void draw(ShaderRef& shader);

    static MeshRef Create(const MeshCreateParams& params);

private:
    Mesh() = delete;
    Mesh(const Mesh& shader) = delete;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureSlot>& textures);

    void setup();

private:
    std::vector<Vertex>       _vertices;
    std::vector<unsigned int> _indices;
    std::vector<TextureSlot>  _textures;
    MeshMaterial              _material;

    VAORef _vao;
};