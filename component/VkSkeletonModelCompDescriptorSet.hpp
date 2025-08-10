#pragma once

#include "global.hpp"
#include "VkSkeletonArrayBuffer.hpp"
#include "VkSkeleton.hpp"
#include "VkKeyFrameArrayBuffer.hpp"

namespace Rx{
    namespace Component{
        struct VkSkeletonModelCompDescriptorSet {
            VkDescriptorSet vkDescriptorSet;
            void createDescriptorSet(VkSkeleton& vkSkeleton, VkKeyFrameArrayBuffer& keyFrameBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer);
            void destroyDescriptorSet();
        };

        void vkSkeletonModelCompDescriptorSet_component_on_add(flecs::entity e, VkSkeleton& vkSkeleton, VkKeyFrameArrayBuffer& keyFrameBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer, VkSkeletonModelCompDescriptorSet& descriptorSet);
        void vkSkeletonModelCompDescriptorSet_component_on_remove(flecs::entity e, VkSkeletonModelCompDescriptorSet& descriptorSet);
    }
}
