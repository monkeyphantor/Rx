#include "VkTransformBuffer.hpp"
#include "TransformInstance.hpp"

namespace Rx {
    namespace Component {

        void transformBuffer_component_on_set(flecs::entity e, VkTransformBuffer& transformBuffer) {
            // Create the transform buffer with the specified number of transforms
            transformBuffer.buffer = Core::createBufferInterface(
                transformBuffer.maxNumberTransforms,
                sizeof(TransformInstance), 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );
        }

        void transformBuffer_component_on_remove(flecs::entity e, VkTransformBuffer& transformBuffer) {
            // Cleanup GPU resources
            Core::destroyBufferInterface(transformBuffer.buffer);
        }
    }
} 