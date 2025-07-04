#pragma once


#include "../core/global.hpp"
#include "Material.hpp"
namespace Rx{
    namespace Component {
        struct ColorModelInstance {
            glm::mat4 transform;
            glm::mat4 normalTransform;
            glm::vec4 albedo;
            glm::vec4 metalRough;       // Base color
            glm::vec4 emissive;
        };
    }
}