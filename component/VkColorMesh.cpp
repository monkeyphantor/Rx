#include "VkColorMesh.hpp"
#include "mutex.hpp"
void mesh_component_on_add(flecs::entity e, Rx::Component::ColorMesh& mesh, Rx::Component::VkColorMesh& colorMesh)
{

    RX_ASSERT(mesh.vertices.size() > 0, "ColorMesh", "mesh_component_on_add", "size of vertices must be greater than 0");
    RX_ASSERT(mesh.indices.size() > 0, "ColorMesh", "mesh_component_on_add", "size of indices must be greater than 0");


    Rx::Core::BufferInterface vertexInterface =
        Rx::Core::createBufferInterface(mesh.vertices.size(), sizeof(Rx::Vertex::Color),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    Rx::Core::BufferInterface indexInterface =
        Rx::Core::createBufferInterface(mesh.indices.size(), sizeof(uint32_t),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    colorMesh.vertexBuffer = Rx::Core::createBuffer(mesh.vertices.size(), sizeof(Rx::Vertex::Color),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    colorMesh.indexBuffer = Rx::Core::createBuffer(mesh.indices.size(), sizeof(uint32_t),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    memcpy(vertexInterface.pMemory, mesh.vertices.data(), mesh.vertices.size() * sizeof(Rx::Vertex::Color));
    memcpy(indexInterface.pMemory, mesh.indices.data(), mesh.indices.size() * sizeof(uint32_t));

    VkBufferCopy vertexCopy;
    vertexCopy.srcOffset = 0;
    vertexCopy.dstOffset = 0;
    vertexCopy.size = mesh.vertices.size() * sizeof(Rx::Vertex::Color);

    VkBufferCopy indexCopy;
    indexCopy.srcOffset = 0;
    indexCopy.dstOffset = 0;
    indexCopy.size = mesh.indices.size() * sizeof(uint32_t);

    RX_VK_MUTEX(
    Rx::Core::beginSingleCommand(Rx::Core::singleCommand);
    vkCmdCopyBuffer(Rx::Core::singleCommand.vkCommandBuffer,
        vertexInterface.vkBuffer,
        colorMesh.vertexBuffer.vkBuffer,
        1, &vertexCopy);
    vkCmdCopyBuffer(Rx::Core::singleCommand.vkCommandBuffer,
        indexInterface.vkBuffer,
        colorMesh.indexBuffer.vkBuffer,
        1, &indexCopy);
    Rx::Core::endSingleCommand(Rx::Core::singleCommand);
    Rx::Core::submitSingleCommand(Rx::Core::singleCommand);)

    Rx::Core::destroyBufferInterface(indexInterface);
    Rx::Core::destroyBufferInterface(vertexInterface);
}

void mesh_component_on_remove(flecs::entity e, Rx::Component::VkColorMesh& mesh)
{
    Rx::Core::destroyBuffer(mesh.indexBuffer);
    Rx::Core::destroyBuffer(mesh.vertexBuffer);
}
