#pragma once
#include "global.hpp"

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
    }
}