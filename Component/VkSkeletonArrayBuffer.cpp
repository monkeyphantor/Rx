#include "VkSkeletonArrayBuffer.hpp"
#include "SkeletonBuffer.hpp"

namespace Rx {
    namespace Component {

          void vkSkeletonArrayBuffer_component_on_set(flecs::entity e, VkSkeletonArrayBuffer& vkSkeletonArrayBuffer){
            // Create the skeleton array buffer with the specified number of skeletons
            vkSkeletonArrayBuffer.buffer = Core::createBufferInterface(
                vkSkeletonArrayBuffer.maxNumberSkeletons * 256, // Assuming max 256 mat4 bones per skeleton
                sizeof(NodeTransform), 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );
        }

        void vkSkeletonArrayBuffer_component_on_remove(flecs::entity e, VkSkeletonArrayBuffer& vkSkeletonArrayBuffer) {
            // Cleanup GPU resources
            Core::destroyBufferInterface(vkSkeletonArrayBuffer.buffer);
        }
    }
} 