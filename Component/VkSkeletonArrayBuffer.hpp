#pragma once

#include "Global.hpp"
#include "Buffer.hpp"
#include "flecs.h"
namespace Rx{
    namespace Component{
        struct VkSkeletonArrayBuffer{
            uint32_t maxNumberSkeletons{0}; 
            uint32_t numberSkeletons{0};
            Core::BufferInterface buffer;
        };



        void vkSkeletonArrayBuffer_component_on_set(flecs::entity e, VkSkeletonArrayBuffer& vkSkeletonArrayBuffer);
        void vkSkeletonArrayBuffer_component_on_remove(flecs::entity e, VkSkeletonArrayBuffer& vkSkeletonArrayBuffer);
    }
}