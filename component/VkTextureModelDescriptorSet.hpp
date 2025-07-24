#pragma once

#include "global.hpp"
#include "flecs.h"
#include "VkTextureMaterialBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureArray.hpp"

namespace Rx{
    namespace Component{
        struct VkTextureModelDescriptorSet {
            VkDescriptorSet vkDescriptorSet; // Descriptor set for Vulkan

            void createDescriptorSet(VkTextureArray& textureArray, VkTextureMaterialBuffer& textureMaterialBuffer, VkTransformBuffer& transformBuffer);
            void destroyDescriptorSet();
        };

        void textureModelDescriptorSet_component_on_add(flecs::entity e, VkTextureArray& textureArray, VkTextureMaterialBuffer& textureMaterialBuffer, VkTransformBuffer& transformBuffer, VkTextureModelDescriptorSet& descriptorSet);
        void textureModelDescriptorSet_component_on_remove(flecs::entity e, VkTextureModelDescriptorSet& descriptorSet);
    }
}