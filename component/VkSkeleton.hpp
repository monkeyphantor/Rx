#pragma once

#include "global.hpp"
#include "buffer.hpp"
#include "flecs.h"

namespace Rx {
namespace Component {
    struct VkSkeletonNode {
        glm::mat4 offset;
        glm::ivec4 meta; // parentIndex, isBone, boneIndex, padding
    };

    struct VkSkeleton {
        uint32_t nodeCount{0};
        Core::BufferInterface buffer;
    };

    void vkSkeleton_component_on_set(flecs::entity e, VkSkeleton& vkSkeleton);
    void vkSkeleton_component_on_remove(flecs::entity e, VkSkeleton& vkSkeleton);
}
}
