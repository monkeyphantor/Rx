#pragma once

#include "Global.hpp"
#include "flecs.h"


namespace Rx
{
    namespace Asset
    {
        struct MainTextureModelHeader{
            uint32_t numMeshes;
            uint32_t numTextures;
        };

        struct TextureMeshHeader{
            char name[64];
            uint32_t numVertices;
            uint32_t numIndices;
        };

        struct TextureHeader{
            char name[64];
            uint64_t size;
            uint32_t width;
            uint32_t height;
        };

        flecs::entity /*assetEntity*/ loadTextureModel(const std::string& filePath, flecs::world& world, const std::string& assetName, const uint32_t maxNumberInstances);
    } // namespace Asset
} // namespace Rx