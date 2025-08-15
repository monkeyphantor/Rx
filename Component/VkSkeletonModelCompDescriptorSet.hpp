#pragma once

#include "Global.hpp"
#include "VkSkeleton.hpp"
#include "VkKeyFrameArrayBuffer.hpp"
#include "VkSkeletonArrayBuffer.hpp"

namespace Rx{
    namespace Component{
        struct VkSkeletonModelCompDescriptorSet {
            VkDescriptorSet vkDescriptorSet;
            void createDescriptorSet(VkSkeleton& skeleton, VkKeyFrameArrayBuffer& keyFrameArrayBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer);
            void destroyDescriptorSet();
        };

        void vkSkeletonModelCompDescriptorSet_component_on_add(flecs::entity e, VkSkeleton& skeleton, VkKeyFrameArrayBuffer& keyFrameArrayBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer, VkSkeletonModelCompDescriptorSet& descriptorSet);
        void vkSkeletonModelCompDescriptorSet_component_on_remove(flecs::entity e, VkSkeletonModelCompDescriptorSet& descriptorSet);




    }
}