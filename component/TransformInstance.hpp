#pragma once


#include "global.hpp"

namespace Rx{
    namespace Component {
        struct TransformInstance {
            glm::mat4 transform;
            glm::mat4 normalTransform;
        };
    }
}