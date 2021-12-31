#include "mesh.h"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices) {
    _vertices = vertices;
    _indices = indices;

    setup();
}

/*static*/ MeshRef Mesh::Create(const MeshCreateParams& params) {
    MeshRef mesh(new Mesh(params.vertices, params.indices));

    return mesh;
}

void Mesh::setup() {
    auto vbo = VBO::Create(
        _vertices.data(),
        sizeof(Vertex) * _vertices.size(),
        BufferLayout({
            { BufferItemType::Float3, "position" },
            { BufferItemType::Float3, "normal" },
            { BufferItemType::Float2, "texCoords" }
        })
    );
    auto ibo = IBO::Create(_indices.data(), _indices.size());

    _vao = VAO::Create();
    _vao->addVertextBuffer(vbo);
    _vao->setIndexBuffer(ibo);
}

void Mesh::draw(ShaderRef& shader) {
    shader->use();
    _vao->bind();
    glDrawElements(GL_TRIANGLES, _vao->indexCount(), GL_UNSIGNED_INT, 0);
}
