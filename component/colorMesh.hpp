#pragma once

#include "../core/global.hpp"
#include "../core/queue.hpp"
#include "../core/command.hpp"
#include "../core/buffer.hpp"
#include "mesh.hpp"
#include "../flecs/include/flecs.h"
namespace Rx
{

    namespace Component
    {
        struct ColorMesh
        {
        public:
     
            Core::Buffer vertexBuffer;
            Core::Buffer indexBuffer;

        public:
            const VkBuffer& getVertexBuffer()
            {
                return vertexBuffer.vkBuffer;
            }

            const VkBuffer& getIndexBuffer()
            {
                return indexBuffer.vkBuffer;
            }

            const uint32_t& getNumberIndices()
            {
                return indexBuffer.numberElements;    
            }
        };

    }
}

void mesh_component_on_add(flecs::entity e, Rx::Component::Mesh& mesh, Rx::Component::ColorMesh& colorMesh);
void mesh_component_on_remove(flecs::entity e, Rx::Component::ColorMesh& colorMesh);