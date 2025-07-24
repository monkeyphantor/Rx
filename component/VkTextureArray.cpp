#include "VkTextureArray.hpp"

namespace Rx{
    namespace Component{
        void vkTextureArray_component_on_add(flecs::entity e, TextureArray& textureArray, VkTextureArray& vkTextureArray){
            for(const auto& texture : textureArray.textures){
                vkTextureArray.textures.push_back(Core::createTexture(texture.width, texture.height, texture.pixels));
            }
        }

        void vkTextureArray_component_on_remove(flecs::entity e, VkTextureArray& vkTextureArray){
            // Cleanup Vulkan resources
            for(auto& texture : vkTextureArray.textures){
                Core::destroyTexture(texture);
            }
        }
    }
}