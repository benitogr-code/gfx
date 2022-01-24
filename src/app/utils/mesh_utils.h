#pragma once

#include "core/graphics/mesh.h"

namespace MeshUtils {
    void CalculateTangent(Vertex& v1, Vertex& v2, Vertex& v3) {
      auto edge1 = v2.position-v1.position;
      auto edge2 = v3.position-v1.position;
      auto deltaUV1 = v2.texCoords-v1.texCoords;
      auto deltaUV2 = v3.texCoords-v1.texCoords;

      const float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
      auto tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;
      v1.tangent = v2.tangent = v3.tangent = tangent;
    }

    MeshRef CreateCube(float size) {
        const float s = size * 0.5f;

        MeshCreateParams params;
        params.vertices = {
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) }, // Back
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

            { glm::vec3(-s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) }, // Front
            { glm::vec3( s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3(-s,  s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3(-s, -s,  s), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

            { glm::vec3(-s,  s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) }, // Left
            { glm::vec3(-s,  s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3(-s, -s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
            { glm::vec3(-s, -s, -s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
            { glm::vec3(-s, -s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3(-s,  s,  s), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },

            { glm::vec3( s,  s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) }, // Right
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3( s,  s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( s, -s, -s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3( s,  s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3( s, -s,  s), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },

            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) }, // Bottom
            { glm::vec3( s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3(-s, -s,  s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3(-s, -s, -s), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },

            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) }, // Top
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
            { glm::vec3( s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3(-s,  s, -s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
            { glm::vec3(-s,  s,  s), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
        };

        for (int i = 0; i < params.vertices.size(); i+=3) {
          CalculateTangent(params.vertices[i], params.vertices[i+1], params.vertices[i+2]);
        }

        return Mesh::Create(params);
    }

    MeshRef CreateGroundPlane(float cellSize, uint32_t cellCount) {
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

        for (int i = 0; i < params.indices.size(); i+=3) {
          auto idx0 = params.indices[i];
          auto idx1 = params.indices[i+1];
          auto idx2 = params.indices[i+2];
          CalculateTangent(params.vertices[idx0], params.vertices[idx1], params.vertices[idx2]);
        }

        return Mesh::Create(params);
    }
};
