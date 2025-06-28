#pragma once
#include "global.hpp"

namespace Rx
{
    namespace Core
    {
        struct Vertex
        {
            struct Color
            {
                glm::vec3 position;
                glm::vec3 normal;
                glm::vec4 color;
            };
        };
    }
}