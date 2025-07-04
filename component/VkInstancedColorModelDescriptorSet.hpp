#pragma once

#include "../core/buffer.hpp"
#include "../flecs/include/flecs.h"
#include "meshArray.hpp"
#include "VkInstancedColorModelBuffer.hpp"

namespace Rx
{
    namespace Component
    {
        struct VkInstancedColorModelDescriptorSet
        {
            VkDescriptorSet vkDescriptorSet; // Descriptor set for Vulkan

            void createDescriptorSet(Core::BufferInterface& instanceBuffer);
            void destroyDescriptorSet();
        };

    void instancedColorGraphics_component_on_add(flecs::entity e, Rx::Component::VkInstancedColorModelBuffer& colorArray, Rx::Component::VkInstancedColorModelDescriptorSet& graphics);
    void instancedColorGraphics_component_on_remove(flecs::entity e, Rx::Component::VkInstancedColorModelDescriptorSet& graphics);
}
}
