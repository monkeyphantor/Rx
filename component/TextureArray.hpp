#pragma once

#include "image.hpp"
#include "flecs.h"
namespace Rx
{
    namespace Component
    {
        struct TextureArray
        {
            std::vector<Core::TextureCPU> textures;
            std::unordered_map<std::string, uint32_t> textureMap;

            void addTexture(const std::string& name, const Core::TextureCPU& texture)
            {
                textures.push_back(texture);
                textureMap[name] = textures.size() - 1;
            }

            uint32_t getTextureIndex(const std::string& name) const
            {
                auto it = textureMap.find(name);
                if (it != textureMap.end())
                {
                    return it->second;
                }
                RX_LOGE("TextureArray", "getTextureIndex", ("Texture not found: " + name).c_str());
            }
        };

        void textureArray_component_on_remove(flecs::entity e, TextureArray& textureArray);

    }
}