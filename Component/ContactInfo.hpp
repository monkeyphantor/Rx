#pragma once

#include "Global.hpp"

namespace Rx{
    namespace Component{
        struct ContactInfo{
            glm::vec3 position; // Contact point in world space
            glm::vec3 normal;   // Contact normal in world space
        };
    }
}