#include "mesh.h"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureSlot>& textures) {
    _vertices = vertices;
    _indices = indices;
    _textures = textures;

    setup();
}

/*static*/ MeshRef Mesh::Create(const MeshCreateParams& params) {
    MeshRef mesh(new Mesh(params.vertices, params.indices, params.textures));

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

    _vao = VAO::Create();
    _vao->addVertextBuffer(vbo);

    if(_indices.size() > 0) {
         auto ibo = IBO::Create(_indices.data(), _indices.size());
        _vao->setIndexBuffer(ibo);
    }
}

void Mesh::draw(ShaderRef& shader) {
    const auto textureCount = _textures.size();
    shader->setUniformInt("texture_enable", textureCount > 0 ? 1 : 0);

    if (textureCount > 0) {
        for(int i = 0; i < textureCount; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            shader->setUniformInt(_textures[i].slot.c_str(), i);
            glBindTexture(GL_TEXTURE_2D, _textures[i].texture->id());
        }
    }
    else {
        shader->setUniformVec3("mat_color", _material.color);
        shader->setUniformFloat("mat_ambient", _material.ambientFactor);
        shader->setUniformFloat("mat_specular", _material.specularFactor);
    }

    _vao->bind();

    if (_vao->indexCount() > 0) {
        glDrawElements(GL_TRIANGLES, _vao->indexCount(), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    }
}
