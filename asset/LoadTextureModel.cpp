#include "LoadTextureModel.hpp"
#include "LoadDataHelper.hpp"
#include "TextureMeshArray.hpp"
#include "TextureMaterialBuffer.hpp"
#include "IndirectBuffer.hpp"
#include "TextureArray.hpp"
#include "Material.hpp"
#include "VkTextureMeshArray.hpp"
#include "VkTextureMaterialBuffer.hpp"
#include "VkIndirectBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureArray.hpp"
#include "VkTextureModelDescriptorSet.hpp"
#include "gameWorldBase.hpp"

namespace Rx {
    namespace Asset {

        flecs::entity loadTextureModel(const std::string& filePath, flecs::world& world, const std::string& assetName, const uint32_t maxNumberInstances) {
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                RX_LOGE("loadTextureModel", "FileIO", ("Failed to open file " + filePath).c_str());
                return flecs::entity::null(world);
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), size)) {
                RX_LOGE("loadTextureModel", "FileIO", ("Failed to read file " + filePath).c_str());
                return flecs::entity::null(world);
            }

            uint64_t offset = 0;

            MainTextureModelHeader header;
            loadData(buffer, header.numMeshes, offset);
            loadData(buffer, header.numTextures, offset);

            auto asset = world.entity(assetName.c_str());
            Rx::Component::TextureMeshArray meshArray;
            Rx::Component::TextureMaterialBuffer textureMaterialBuffer;
            Rx::Component::IndirectBuffer indirectBuffer;
            Rx::Component::TextureArray textureArray;

            for (uint32_t i = 0; i < header.numMeshes; i++) {
                Rx::Component::TextureMaterial material;
                loadData(buffer, material.textureIndex, offset);
                loadData(buffer, material.albedo, offset);
                loadData(buffer, material.metalRough, offset);
                loadData(buffer, material.emissive, offset);
                textureMaterialBuffer.addMaterial(material);
            }

            for (uint32_t i = 0; i < header.numMeshes; i++) {
                TextureMeshHeader meshHeader;
                loadString(buffer, meshHeader.name, 64, offset);
                loadData(buffer, meshHeader.numVertices, offset);
                loadData(buffer, meshHeader.numIndices, offset);

                std::vector<Rx::Vertex::Texture> vertices(meshHeader.numVertices);
                for(uint32_t j = 0; j < meshHeader.numVertices; j++){
                    loadData(buffer, vertices[j].position, offset);
                    loadData(buffer, vertices[j].normal, offset);
                    loadData(buffer, vertices[j].texCoord, offset);
                }
                
                std::vector<uint32_t> indices(meshHeader.numIndices);
                memcpy(indices.data(), buffer.data() + offset, meshHeader.numIndices * sizeof(uint32_t));
                offset += meshHeader.numIndices * sizeof(uint32_t);

                meshArray.addMesh(meshHeader.name, vertices, indices);
                indirectBuffer.addCommand(meshArray.getCommand(meshHeader.name));
            }

            std::vector<TextureHeader> textureHeaders(header.numTextures);
            for (uint32_t i = 0; i < header.numTextures; i++) {
                loadString(buffer, textureHeaders[i].name, 64, offset);
                loadData(buffer, textureHeaders[i].size, offset);
                loadData(buffer, textureHeaders[i].width, offset);
                loadData(buffer, textureHeaders[i].height, offset);
            }

            std::vector<Rx::Core::TextureCPU> texturesCpu(header.numTextures);
            for (uint32_t i = 0; i < header.numTextures; i++) {
                auto& textureCpu = texturesCpu[i];
                textureCpu.width = textureHeaders[i].width;
                textureCpu.height = textureHeaders[i].height;
				textureCpu.pixels = malloc(textureHeaders[i].size);
                for(int j = 0; j < textureHeaders[i].size; j++){
                    loadData(buffer, ((char*)textureCpu.pixels)[j], offset);
                }
                textureCpu.vkFormat = VK_FORMAT_R8G8B8A8_SRGB;
                textureArray.addTexture(textureHeaders[i].name, textureCpu);
            }

            asset.set<Rx::Component::TextureMeshArray>(meshArray);
            asset.set<Rx::Component::TextureMaterialBuffer>(textureMaterialBuffer);
            asset.set<Rx::Component::IndirectBuffer>(indirectBuffer);
            asset.set<Rx::Component::TextureArray>(textureArray);

            asset.add<Rx::Component::VkTextureMeshArray>();
            asset.add<Rx::Component::VkTextureMaterialBuffer>();
            
            Rx::Component::VkIndirectBuffer vkIndirectBuffer;
            vkIndirectBuffer.maxNumberCommands = header.numMeshes;
            vkIndirectBuffer.numberCommands = 0;
            asset.set<Rx::Component::VkIndirectBuffer>(vkIndirectBuffer);

            asset.add<Rx::Component::VkTextureArray>();

            Rx::Component::VkTransformBuffer transformBuffer;
            transformBuffer.maxNumberTransforms = maxNumberInstances;
            transformBuffer.numberTransforms = 0;
            asset.set<Rx::Component::VkTransformBuffer>(transformBuffer);

            asset.add<Rx::Component::VkTextureModelDescriptorSet>();
            asset.add<Rx::ShouldBeUpdated>();

            return asset;
        }

    } // namespace Asset
} // namespace Rx