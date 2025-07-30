#pragma once

#include "global.hpp"
#include "flecs.h"


namespace Rx {
    namespace Component {
        struct SkeletonInstance {
            std::vector<flecs::entity> nodeEntities;
        };
    }
}