#pragma once

#include "../core/global.hpp"
#include "../core/buffer.hpp"
#include "../flecs/include/flecs.h"

namespace Rx{
    namespace Component{
        struct IndirectBuffer{
            uint32_t maxNumberCommands{0}; 
            uint32_t numberCommands{0};
            Core::BufferInterface buffer;
        };



        void indirectBuffer_component_on_set(flecs::entity e, IndirectBuffer& indirectBuffer);
        void indirectBuffer_component_on_remove(flecs::entity e, IndirectBuffer& indirectBuffer);
    }
}