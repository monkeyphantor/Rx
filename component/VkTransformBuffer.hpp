#pragma once

#include "global.hpp"
#include "buffer.hpp"
#include "flecs.h"

namespace Rx{
    namespace Component{
        struct VkTransformBuffer{
            uint32_t maxNumberTransforms{0}; 
            uint32_t numberTransforms{0};
            Core::BufferInterface buffer;
        };



        void transformBuffer_component_on_set(flecs::entity e, VkTransformBuffer& transformBuffer);
        void transformBuffer_component_on_remove(flecs::entity e, VkTransformBuffer& transformBuffer);
    }
}