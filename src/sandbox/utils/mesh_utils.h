#pragma once

#include "system/graphics/mesh.h"

class MeshUtils {
public:
    static MeshRef CreateCube(float size) {
        const float s = size * 0.5f;

        MeshCreateParams params;
        params.vertices = {
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },

            { glm::vec3(-s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },
            { glm::vec3(-s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },
            { glm::vec3(-s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2() },

            { glm::vec3(-s,  s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2() },

            { glm::vec3( s,  s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },

            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },

            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
        };

        return Mesh::Create(params);
    }
};
