#pragma once

#include "global.hpp"
#include "buffer.hpp"
#include "flecs.h"
#include "TextureMaterialBuffer.hpp"
namespace Rx{
    namespace Component{
        struct VkTextureMaterialBuffer{
            uint32_t maxNumberMaterials{0}; 
            uint32_t numberMaterials{0};
            Core::BufferInterface buffer;
        };



        void textureMaterialBuffer_component_on_add(flecs::entity e, TextureMaterialBuffer& textureMaterialBuffer, VkTextureMaterialBuffer& vkTextureMaterialBuffer);
        void textureMaterialBuffer_component_on_remove(flecs::entity e, VkTextureMaterialBuffer& textureMaterialBuffer);
    }
}