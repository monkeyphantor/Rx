#pragma once

#include "global.hpp"
#include "flecs.h"
#include "TextureMeshArray.hpp"
#include "buffer.hpp"

namespace Rx{
    namespace Component{
        struct VkTextureMeshArray{

            Core::Buffer vertexBuffer;
            Core::Buffer indexBuffer;
        };



        void vkTextureMeshArray_component_on_add(flecs::entity e, TextureMeshArray& meshArray, VkTextureMeshArray& vkTextureMeshArray);
        void vkTextureMeshArray_component_on_remove(flecs::entity e, VkTextureMeshArray& vkTextureMeshArray);
    }
}