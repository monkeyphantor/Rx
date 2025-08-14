#pragma once
#include "global.hpp"
#include "flecs.h"
#include "Node.hpp"

namespace Rx {
    namespace Component {
        struct VkNode{
            glm::ivec4 isBone_parentIndex_boneIndex;
            glm::mat4 offset; 
        };

        struct Skeleton{
            std::vector<Node> nodes;
            std::vector<VkNode> vkNodes;
            std::vector<NodeName> nodeNames;
            flecs::entity animationPrefab;
        };


        void skeleton_on_component_set(flecs::entity entity, Skeleton& skeleton);
    }
}