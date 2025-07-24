#include "LoadTexture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Rx {
    namespace Asset {

        Core::TextureCPU loadTexture(const std::string& filePath) {
            // Load the texture from the specified file path
            Core::TextureCPU texture;
            int width, height, numberChannels;
            texture.pixels = stbi_load(filePath.c_str(), &width, &height, &numberChannels, STBI_rgb_alpha);
            RX_ASSERT(texture.pixels, "Asset", "loadTexture", ("Failed to load texture from file: " + filePath).c_str());
            texture.width = width;
            texture.height = height;
            return texture;
        }
    } // namespace Asset
} // namespace Rx