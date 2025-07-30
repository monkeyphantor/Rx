#pragma once
#include "global.hpp"
#include "vertex.hpp"
namespace Rx
{

    namespace Component
    {
        struct SkeletonMeshArray
        {
            int firstIndex{0};
            int vertexOffset{0};
            std::vector<std::vector<Vertex::Skeleton>> vertexArrays; 
            std::vector<std::vector<uint32_t>> indexArrays; 
            std::unordered_map<std::string, VkDrawIndexedIndirectCommand> meshNameToCommand;
            void addMesh(const std::string& name, const std::vector<Vertex::Skeleton>& vertices, const std::vector<uint32_t>& indices);
            VkDrawIndexedIndirectCommand getCommand(const std::string& name) const
            {
                auto it = meshNameToCommand.find(name);
                if (it != meshNameToCommand.end())
                {
                    return it->second;
                }
                RX_LOGE("SkeletonMeshArray", "getCommand", ("Mesh not found: " + name).c_str());
            }
        };
    }
}