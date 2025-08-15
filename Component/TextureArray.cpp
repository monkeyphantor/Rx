#include "TextureArray.hpp"


namespace Rx
{
    namespace Component
    {
        void textureArray_component_on_remove(flecs::entity e, TextureArray& textureArray)
        {
            // Cleanup textures in the TextureArray
            for (auto& texture : textureArray.textures)
            {
                free(texture.pixels); // Free the pixel data
            }
            textureArray.textures.clear();
            textureArray.textureMap.clear();
        }
    }
}