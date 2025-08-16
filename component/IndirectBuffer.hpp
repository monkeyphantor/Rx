#pragma once

#include "Global.hpp"

namespace Rx{
    namespace Component{
        struct IndirectBuffer{
            std::vector<VkDrawIndexedIndirectCommand> commands;

            void addCommand(const VkDrawIndexedIndirectCommand& command) {
                commands.push_back(command);
            }

            void setInstanceCount(uint32_t count) {
                for(auto& command : commands) {
                    command.instanceCount = count;
                }
            }

            void addInstanceCount(uint32_t count) {
                for(auto& command : commands) {
                    command.instanceCount += count;
                }
            }
        };
    }
}