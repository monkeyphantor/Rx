#pragma once

#include "Global.hpp"
#include "flecs.h"

namespace Rx{
    namespace Component{
        struct AnimationMap {
            std::unordered_map<std::string, flecs::entity> animations;
        };
    }
}