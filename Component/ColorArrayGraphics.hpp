#pragma once

#include "../Core/Buffer.hpp"
#include "../Flecs/include/flecs.h"
#include "MeshArray.hpp"
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
