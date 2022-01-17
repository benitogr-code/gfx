#pragma once

#include "core/graphics/mesh.h"

class MeshUtils {
public:
    static MeshRef CreateCube(float size) {
        const float s = size * 0.5f;

        MeshCreateParams params;
        params.vertices = {
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2() },

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
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2() },

            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2() },

            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
            { glm::vec3(-s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2() },
        };

        return Mesh::Create(params);
    }

    static MeshRef CreateGroundPlane(float cellSize, uint32_t cellCount) {
        MeshCreateParams params;
        params.vertices.reserve((cellCount+1)*(cellCount+1));
        params.indices.reserve(cellCount*cellCount*6);

        const float xStart = -((float)cellCount / 2.0f) * cellSize;
        const float zStart = -xStart;

        for(uint32_t i = 0; i <= cellCount; ++i) {
            for (uint32_t j = 0; j <= cellCount; ++j) {
                params.vertices.push_back(Vertex({
                    glm::vec3(xStart + (j*cellSize), 0.0f, zStart - (i*cellSize)),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec2()
                }));
            }
        }

        for(uint32_t i = 0; i < cellCount; ++i) {
            for (uint32_t j = 0; j < cellCount; ++j) {
                const auto row = cellCount+1;
                // 1st triangle
                params.indices.push_back((i*row)+j);
                params.indices.push_back((i*row)+j+1);
                params.indices.push_back(((i+1)*row)+j);

                // 2nd triangle
                params.indices.push_back((i*row)+j+1);
                params.indices.push_back(((i+1)*row)+j+1);
                params.indices.push_back((((i+1)*row)+j));
            }
        }

        return Mesh::Create(params);
    }
};
