#pragma once

#include "../core/buffer.hpp"
#include "../flecs/include/flecs.h"
#include "meshArray.hpp"
#include "VkInstancedColorModelBuffer.hpp"

namespace Rx
{
    namespace Component
    {
        struct ColorArrayGraphics
        {
            VkDescriptorSet vkDescriptorSet; // Descriptor set for Vulkan

            void createDescriptorSet(Core::BufferInterface& instanceBuffer);
            void destroyDescriptorSet();
        };

    void colorArrayGraphics_component_on_add(flecs::entity e, Rx::Component::VkInstancedColorModelBuffer& colorArray, Rx::Component::ColorArrayGraphics& graphics);
    void colorArrayGraphics_component_on_remove(flecs::entity e, Rx::Component::ColorArrayGraphics& graphics);
}
}
