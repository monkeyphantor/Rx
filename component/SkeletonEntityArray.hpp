#pragma once

#include "Global.hpp"
#include "flecs.h"

namespace Rx{
    namespace Component{
        struct SkeletonEntityArray {
            std::vector<flecs::entity> skeletons;
        };
    }
}