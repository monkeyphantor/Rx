#pragma once

#include "../Core/Global.hpp"
#include "../Flecs/include/flecs.h"
#include "MeshArray.hpp"
#include "../Core/Buffer.hpp"
namespace Rx{
    namespace Component{
        struct ColorMeshArray{

            Core::Buffer vertexBuffer;
            Core::Buffer indexBuffer;
        };



        void colorMeshArray_component_on_add(flecs::entity e, MeshArray& meshArray, ColorMeshArray& colorMeshArray);
        void colorMeshArray_component_on_remove(flecs::entity e, ColorMeshArray& colorMeshArray);
    }
}