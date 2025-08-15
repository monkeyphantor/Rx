#pragma once
#include "../Core/Global.hpp"

namespace Rx
{
    namespace Core
    {
 
        void createPipelineLayouts();
        void destroyPipelineLayouts();

        inline VkPipelineLayout colorMeshPipelineLayout;
        void createColorMeshPipelineLayout();
        void destroyColorMeshPipelineLayout();

        inline VkPipelineLayout colorMeshArrayPipelineLayout;
        void createColorMeshArrayPipelineLayout();
        void destroyColorMeshArrayPipelineLayout();

        inline VkPipelineLayout instancedColorMeshPipelineLayout;
        void createInstancedColorMeshPipelineLayout();
        void destroyInstancedColorMeshPipelineLayout();

        inline VkPipelineLayout textureModelPipelineLayout;
        void createTextureModelPipelineLayout();
        void destroyTextureModelPipelineLayout();

        inline VkPipelineLayout skeletonModelPipelineLayout;
        void createSkeletonModelPipelineLayout();
        void destroySkeletonModelPipelineLayout();

        inline VkPipelineLayout skeletonModelCompPipelineLayout;
        void createSkeletonModelCompPipelineLayout();
        void destroySkeletonModelCompPipelineLayout();
    }
}