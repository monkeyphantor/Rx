#pragma once

#include "Global.hpp"
#include "Buffer.hpp"
#include "flecs.h"
#include "IndirectBuffer.hpp"
namespace Rx{
    namespace Component{
        struct VkIndirectBuffer{
            uint32_t maxNumberCommands{0}; 
            uint32_t numberCommands{0};
            Core::BufferInterface buffer;

            void copyFrom(const IndirectBuffer& indirectBuffer){
                if(indirectBuffer.commands.size() > maxNumberCommands) {
                    RX_LOGE("VkIndirectBuffer", "copyFrom", "Indirect buffer commands exceed max number of commands");
                }
                numberCommands = indirectBuffer.commands.size();
                
                memcpy(buffer.pMemory, indirectBuffer.commands.data(), numberCommands * sizeof(VkDrawIndexedIndirectCommand));
            }
        };



        void indirectBuffer_component_on_set(flecs::entity e, VkIndirectBuffer& indirectBuffer);
        void indirectBuffer_component_on_remove(flecs::entity e, VkIndirectBuffer& indirectBuffer);
    }
}