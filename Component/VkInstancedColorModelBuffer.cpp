#include "VkInstancedColorModelBuffer.hpp"
#include "ColorModelInstance.hpp"

namespace Rx {
    namespace Component {

        void colorMeshInstanceBuffer_on_set(flecs::entity e, VkInstancedColorModelBuffer& instanceBuffer) {
            // Create the instance buffer with the specified number of instances
            instanceBuffer.instanceBuffer = Core::createBufferInterface(
                instanceBuffer.maxNumberInstances,
                sizeof(ColorModelInstance),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            );
        }

void colorMeshInstanceBuffer_on_remove(flecs::entity e, Rx::Component::VkInstancedColorModelBuffer& instanceBuffer) {
    // Cleanup the instance buffer
    Core::destroyBufferInterface(instanceBuffer.instanceBuffer);
}

    } // namespace Component
} // namespace Rx