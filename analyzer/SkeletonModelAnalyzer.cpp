#include "LoadDataHelper.hpp"
#include "SkeletonModelAnalyzer.hpp"
#include "SkeletonMeshArray.hpp"
#include "TextureMaterialBuffer.hpp"
#include "IndirectBuffer.hpp"
#include "TextureArray.hpp"
#include "Material.hpp"
#include "flecs.h"

namespace Rx {
    namespace Analyzer {
        void analyzeSkeleton(std::string filePath) {
            auto [nodes, nodeNames] = getNodes(filePath);
            printSkeletonNodes(nodes, nodeNames);
        }


       std::pair<std::vector<Component::Node>, std::vector<Component::NodeName>> getNodes(const std::string& filePath){
            
            // Read the entire file into a vector<char>
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file) {
            RX_LOGE("Asset", "convertToTextureModel", ("Failed to open skeleton model file: " + filePath).c_str());
        }
        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> data(static_cast<size_t>(fileSize));
        if (!file.read(data.data(), fileSize)) {
            RX_LOGE("Asset", "convertToTextureModel", ("Failed to open skeleton model file: " + filePath).c_str());
        }

        uint64_t offset = 0;
        MainSkeletonModelHeader header;

        loadData(data, header.numberMeshes, offset);
        loadData(data, header.numberNodes, offset);
        loadData(data, header.numberBones, offset);
        loadData(data, header.numberAnimations, offset);
        loadData(data, header.numberTextures, offset);

        Rx::Component::SkeletonMeshArray meshArray;
        Rx::Component::TextureMaterialBuffer textureMaterialBuffer;
        Rx::Component::IndirectBuffer indirectBuffer;
        Rx::Component::TextureArray textureArray;

        for (uint32_t i = 0; i < header.numberMeshes; i++) {
            Rx::Component::TextureMaterial material;
            loadData(data, material.textureIndex, offset);
            loadData(data, material.albedo, offset);
            loadData(data, material.metalRough, offset);
            loadData(data, material.emissive, offset);
            textureMaterialBuffer.addMaterial(material);
        }

        for (uint32_t i = 0; i < header.numberMeshes; i++) {
            SkeletonMeshHeader meshHeader;
            loadString(data, meshHeader.name, 64, offset);
            loadData(data, meshHeader.numVertices, offset);
            loadData(data, meshHeader.numIndices, offset);

            std::vector<Rx::Vertex::Skeleton> vertices(meshHeader.numVertices);
            for(uint32_t j = 0; j < meshHeader.numVertices; j++){
                loadData(data, vertices[j].position, offset);
                loadData(data, vertices[j].normal, offset);
                loadData(data, vertices[j].texCoord, offset);
                loadData(data, vertices[j].bones, offset);
                loadData(data, vertices[j].weights, offset);
                loadData(data, vertices[j].nodeIndex, offset);
            }
            
            std::vector<uint32_t> indices(meshHeader.numIndices);
            memcpy(indices.data(), data.data() + offset, meshHeader.numIndices * sizeof(uint32_t));
            offset += meshHeader.numIndices * sizeof(uint32_t);

            meshArray.addMesh(meshHeader.name, vertices, indices);
            indirectBuffer.addCommand(meshArray.getCommand(meshHeader.name));
        }
        

        std::vector<TextureHeader> textureHeaders(header.numberTextures);
        for (uint32_t i = 0; i < header.numberTextures; i++) {
            loadString(data, textureHeaders[i].name, 64, offset);
            loadData(data, textureHeaders[i].size, offset);
            loadData(data, textureHeaders[i].width, offset);
            loadData(data, textureHeaders[i].height, offset);
        }

        std::vector<Rx::Core::TextureCPU> texturesCpu(header.numberTextures);
        for (uint32_t i = 0; i < header.numberTextures; i++) {
            auto& textureCpu = texturesCpu[i];
            textureCpu.width = textureHeaders[i].width;
            textureCpu.height = textureHeaders[i].height;
            textureCpu.pixels = malloc(textureHeaders[i].size);
            for(int j = 0; j < textureHeaders[i].size; j++){
                loadData(data, ((char*)textureCpu.pixels)[j], offset);
            }
            textureCpu.vkFormat = VK_FORMAT_R8G8B8A8_SRGB;
            textureArray.addTexture(textureHeaders[i].name, textureCpu);
        }

        std::vector<Component::Node> nodes(header.numberNodes);
        std::vector<Component::NodeName> nodeNames(header.numberNodes);

        for(uint32_t i = 0; i < nodes.size(); i++){
            loadData(data, nodes[i].isBone, offset);
            loadString(data, nodeNames[i].name, 128, offset);
            loadData(data, nodes[i].offset, offset);
            loadData(data, nodes[i].numberChildren, offset);
			loadData(data, nodes[i].nodeIndex, offset);
            loadData(data, nodes[i].boneIndex, offset);
        }
            return {nodes, nodeNames};
        }


        void printNodeRecursive(const std::vector<Component::Node>& nodes, const std::vector<Component::NodeName>& nodeNames, uint32_t& nodeIndex, const std::string& indent) {
            // Print current node
            std::cout << indent << "Node: " << nodeNames[nodeIndex].name << " ( NodeIndex: " << nodes[nodeIndex].nodeIndex  << " BoneIndex: "<< nodes[nodeIndex].boneIndex << " )" << std::endl;

            // Recursively print children
            uint32_t currentNodeIndex = nodeIndex;
            for (uint32_t i = 0; i < nodes[currentNodeIndex].numberChildren; ++i) {
                printNodeRecursive(nodes, nodeNames, ++nodeIndex, indent + "  ");
            }
        }

        void printSkeletonNodes(const std::vector<Component::Node>& nodes, const std::vector<Component::NodeName>& nodeNames){
            if (nodes.empty()) {
                return;
            }
            uint32_t nodeIndex = 0;
            printNodeRecursive(nodes, nodeNames, nodeIndex, "");
        }
    }
}