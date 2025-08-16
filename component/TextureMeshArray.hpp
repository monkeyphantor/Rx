#pragma once
#include "Global.hpp"
#include "Vertex.hpp"
namespace Rx
{

    namespace Component
    {
        struct TextureMeshArray
        {
            int firstIndex{0};
            int vertexOffset{0};
            std::vector<std::vector<Vertex::Texture>> vertexArrays; 
            std::vector<std::vector<uint32_t>> indexArrays; 
            std::unordered_map<std::string, VkDrawIndexedIndirectCommand> meshNameToCommand;
            void addMesh(const std::string& name, const std::vector<Vertex::Texture>& vertices, const std::vector<uint32_t>& indices);
            VkDrawIndexedIndirectCommand getCommand(const std::string& name) const
            {
                auto it = meshNameToCommand.find(name);
                if (it != meshNameToCommand.end())
                {
                    return it->second;
                }
                RX_LOGE("TextureMeshArray", "getCommand", ("Mesh not found: " + name).c_str());
            }
        };
    }
}