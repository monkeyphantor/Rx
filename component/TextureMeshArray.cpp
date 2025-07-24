#include "TextureMeshArray.hpp"

namespace Rx
{
    namespace Component
    {
        void TextureMeshArray::addMesh(const std::string& name, const std::vector<Vertex::Texture>& vertices, const std::vector<uint32_t>& indices)
        {
            if (meshNameToCommand.find(name) != meshNameToCommand.end())
            {
                RX_LOGE("TextureMeshArray", "addMesh", ("TextureMesh with name " + name + " already exists.").c_str());
            }

            VkDrawIndexedIndirectCommand command{};
            command.indexCount = static_cast<uint32_t>(indices.size());
            command.firstIndex = firstIndex;
            command.vertexOffset = vertexOffset;
            command.instanceCount = 1; 
            command.firstInstance = 0; 

            firstIndex += static_cast<int>(indices.size());
            vertexOffset += static_cast<int>(vertices.size());

            meshNameToCommand[name] = command;

            vertexArrays.push_back(vertices);
            indexArrays.push_back(indices);
        }
    }
}