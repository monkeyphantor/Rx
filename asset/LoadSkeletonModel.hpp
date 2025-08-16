#pragma once
#include "Global.hpp"
#include "flecs.h"
namespace Rx {
    namespace Asset {

        struct MainSkeletonModelHeader{
            uint32_t numberMeshes;
            uint32_t numberNodes;
            uint32_t numberBones;
            uint32_t numberAnimations;
            uint32_t numberTextures;
        };

        struct SkeletonMeshHeader{
            char name[64];
            uint32_t numVertices;
            uint32_t numIndices;
        };

        struct AnimationHeader{
            char name[128];
            float duration;
            float ticksPerSecond;
        };

        struct AnimationBoneHeader{
            char name[128];
            uint32_t numPositionKeys;
            uint32_t numRotationKeys;
            uint32_t numScalingKeys;
        };

        flecs::entity /*assetEntity*/ loadSkeletonModel(const std::string& filePath, flecs::world& world, const std::string& assetName, const uint32_t maxNumberInstances);
        flecs::entity createSkeletonModelInstance(flecs::world& world, const flecs::entity& asset, const std::string& name);

    }

}