#pragma once

#include "Global.hpp"
#include "Queue.hpp"
#include "Command.hpp"
#include "Buffer.hpp"
#include "ColorMesh.hpp"
#include "flecs.h"
namespace Rx
{

    namespace Component
    {
        struct VkColorMesh
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

void mesh_component_on_add(flecs::entity e, Rx::Component::ColorMesh& mesh, Rx::Component::VkColorMesh& colorMesh);
void mesh_component_on_remove(flecs::entity e, Rx::Component::VkColorMesh& colorMesh);