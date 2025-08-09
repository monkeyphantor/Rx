#include "VkKeyFrameArrayBuffer.hpp"

namespace Rx {
    namespace Component {

          void vkKeyFrameArrayBuffer_component_on_set(flecs::entity e, VkKeyFrameArrayBuffer& vkKeyFrameArrayBuffer){
            // Create the key frame array buffer with the specified number of key frames
            vkKeyFrameArrayBuffer.buffer = Core::createBufferInterface(
                vkKeyFrameArrayBuffer.maxNumberKeyFrameArrays * 256,
                sizeof(VkKeyFrame),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );
        }

        void vkKeyFrameArrayBuffer_component_on_remove(flecs::entity e, VkKeyFrameArrayBuffer& vkKeyFrameArrayBuffer) {
            // Cleanup GPU resources
            Core::destroyBufferInterface(vkKeyFrameArrayBuffer.buffer);
        }
        
    }
} 