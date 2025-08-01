#pragma once
#include "global.hpp"
#include "flecs.h"
#include "Node.hpp"

namespace Rx {
    namespace Component {

        struct Skeleton{
            std::vector<Node> nodes;
            flecs::entity animationPrefab;
        };
    }
}