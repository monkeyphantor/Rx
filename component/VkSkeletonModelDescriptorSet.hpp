#pragma once

#include "global.hpp"
#include "flecs.h"
#include "VkTextureMaterialBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureArray.hpp"
#include "VkSkeletonArrayBuffer.hpp"

namespace Rx{
    namespace Component{
        struct VkSkeletonModelDescriptorSet {
            VkDescriptorSet vkDescriptorSet; // Descriptor set for Vulkan

            void createDescriptorSet(VkTextureArray& textureArray, VkTextureMaterialBuffer& textureMaterialBuffer, VkTransformBuffer& transformBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer);
            void destroyDescriptorSet();
        };

        void vkSkeletonModelDescriptorSet_component_on_add(flecs::entity e, VkTextureArray& textureArray, VkTextureMaterialBuffer& textureMaterialBuffer, VkTransformBuffer& transformBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer, VkSkeletonModelDescriptorSet& descriptorSet);
        void vkSkeletonModelDescriptorSet_component_on_remove(flecs::entity e, VkSkeletonModelDescriptorSet& descriptorSet);
    }
}