#pragma once

#include "Global.hpp"

namespace Rx {
    namespace Component {
        struct NodeTransform {
            glm::mat4 bone;
            glm::mat4 local;
        };
    }
}