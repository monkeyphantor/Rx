#pragma once
#include "../core/global.hpp"

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

    }
}