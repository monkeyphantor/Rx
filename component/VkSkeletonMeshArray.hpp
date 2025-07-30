#pragma once

#include "global.hpp"
#include "flecs.h"
#include "SkeletonMeshArray.hpp"
#include "buffer.hpp"

namespace Rx{
    namespace Component{
        struct VkSkeletonMeshArray{
            Core::Buffer vertexBuffer;
            Core::Buffer indexBuffer;
        };



        void vkSkeletonMeshArray_component_on_add(flecs::entity e, SkeletonMeshArray& meshArray, VkSkeletonMeshArray& vkSkeletonMeshArray);
        void vkSkeletonMeshArray_component_on_remove(flecs::entity e, VkSkeletonMeshArray& vkSkeletonMeshArray);
    }
}