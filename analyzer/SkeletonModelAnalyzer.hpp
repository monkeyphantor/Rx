#pragma once

#include "Global.hpp"
#include "Node.hpp"
namespace Rx{
    namespace Analyzer {
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

        struct TextureHeader{
            char name[64];
            uint64_t size;
            uint32_t width;
            uint32_t height;
        };

        void analyzeSkeleton(const std::string filePath);

       std::pair<std::vector<Component::Node>, std::vector<Component::NodeName>> getNodes(const std::string& filePath);

       void printSkeletonNodes(const std::vector<Component::Node>& nodes, const std::vector<Component::NodeName>& nodeNames);
    }
}