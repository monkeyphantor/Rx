#pragma once

#include "global.hpp"
#include "buffer.hpp"
#include "flecs.h"
#include "Skeleton.hpp"

namespace Rx {
    namespace Component {


        struct VkSkeleton {
            Core::BufferInterface skeleton; // Descriptor set for Vulkan
        };

        void vkSkeleton_component_on_add(flecs::entity e, Skeleton& skeleton, VkSkeleton& model);
        void vkSkeleton_component_on_remove(flecs::entity e, VkSkeleton& model);
    }
}