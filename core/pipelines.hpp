#pragma once

#include "../core/global.hpp"


namespace Rx
{
    namespace Core
    {
        void createPipelines();
        void destroyPipelines();

        inline VkPipeline colorMeshPipeline;
        void createColorMeshPipeline();
        void destroyColorMeshPipeline();

        inline VkPipeline colorMeshArrayPipeline;
        void createColorMeshArrayPipeline();
        void destroyColorMeshArrayPipeline();

        inline VkPipeline instancedColorMeshPipeline;
        void createInstancedColorMeshPipeline();
        void destroyInstancedColorMeshPipeline();
    }
}