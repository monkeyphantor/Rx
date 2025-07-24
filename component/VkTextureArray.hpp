#pragma once

#include "global.hpp"
#include "flecs.h"
#include "image.hpp"
#include "TextureArray.hpp"

namespace Rx{
    namespace Component{
        struct VkTextureArray
        {
            std::vector<Core::Texture> textures; // Vulkan textures
        };
        

        void vkTextureArray_component_on_add(flecs::entity e, TextureArray& textureArray, VkTextureArray& vkTextureArray);
        void vkTextureArray_component_on_remove(flecs::entity e, VkTextureArray& vkTextureArray);
    }
}