   #pragma once

#include "global.hpp"
#include "VkSkeletonArrayBuffer.hpp"

namespace Rx{
    namespace Component{
        struct VkKeyFrame{
            glm::vec4 position;
            glm::vec4 rotation;
            glm::vec4 scaling;
        };


        struct VkKeyFrameArrayBuffer {
            uint32_t maxNumberKeyFrameArrays{0};
            uint32_t numberKeyFrameArrays{0};
            Core::BufferInterface buffer;
        };


        void vkKeyFrameArrayBuffer_component_on_set(flecs::entity e, VkKeyFrameArrayBuffer& vkKeyFrameArrayBuffer);
        void vkKeyFrameArrayBuffer_component_on_remove(flecs::entity e, VkKeyFrameArrayBuffer& vkKeyFrameArrayBuffer);
       }
}