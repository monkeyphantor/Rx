#pragma once
#include "Global.hpp"

namespace Rx
{
    namespace Vertex
    {
        struct Color
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec4 color;
        };

        struct Texture{
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;
        };

        struct Skeleton
        {
            glm::vec3 position{};
            glm::vec3 normal{};
            glm::vec2 texCoord{};
            glm::ivec4 bones = glm::ivec4(0);
            glm::vec4 weights = glm::vec4(0.0f);
            int nodeIndex = -1;
        };
    }
}