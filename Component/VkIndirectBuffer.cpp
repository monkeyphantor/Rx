#include "VkIndirectBuffer.hpp"

namespace Rx {
    namespace Component {

        void indirectBuffer_component_on_set(flecs::entity e, VkIndirectBuffer& indirectBuffer) {
            // Create the indirect buffer with the specified number of commands
            indirectBuffer.buffer = Core::createBufferInterface(
                indirectBuffer.maxNumberCommands,
                sizeof(VkDrawIndexedIndirectCommand),
                VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );
        }

        void indirectBuffer_component_on_remove(flecs::entity e, VkIndirectBuffer& indirectBuffer) {
            // Cleanup GPU resources
            Core::destroyBufferInterface(indirectBuffer.buffer);
        }
    }
} // namespace Rx::Component