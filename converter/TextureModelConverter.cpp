#include "TextureModelConverter.hpp"

#include "vertex.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssimpHelpers.hpp"
#include "LoadTextureModel.hpp"
#include "Material.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <filesystem>

namespace Rx
{
    namespace Converter
    {
        void convertToTextureModelFile(std::string pathAssimpFile, std::string pathTextureModelFile)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(pathAssimpFile, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);


            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
                RX_LOGE("convertToTextureModel", "Assimp", importer.GetErrorString())
            }


            Asset::MainTextureModelHeader header;
            std::vector<Rx::Component::TextureMaterial> materials;
            std::vector<Asset::TextureMeshHeader> meshHeaders;
            std::vector<std::vector<Rx::Vertex::Texture>> vertexArrays;
            std::vector<std::vector<uint32_t>> indexArrays;
            std::vector<Asset::TextureHeader> textureHeaders;
            std::vector<char> textureData;

            header.numMeshes = scene->mNumMeshes;
            meshHeaders.resize(header.numMeshes);
            vertexArrays.resize(header.numMeshes);
            indexArrays.resize(header.numMeshes);

            header.numTextures = 0;

            RX_LOGI("convertToTextureModel", "Converting", pathAssimpFile.c_str());
            RX_LOGI("convertToTextureModel", "to", pathTextureModelFile.c_str());
            RX_LOGI("convertToTextureModel", "Number of meshes:", std::to_string(header.numMeshes).c_str());
            RX_LOGI("convertToTextureModel", "Number of materials:", std::to_string(scene->mNumMaterials).c_str());
            RX_LOGI("convertToTextureModel", "Number of textures:", std::to_string(scene->mNumTextures).c_str());

            std::map<int32_t /*materialIndex*/, int32_t /*textureIndex*/> materialMapping;
            std::map<std::string /*texture path*/, int32_t /*textureIndex*/> texturePathMapping;

            for(uint32_t i = 0; i < scene->mNumMaterials; i++){
                aiMaterial* material = scene->mMaterials[i];
                aiString texturePath;
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                    std::cout << "Texture path: " << texturePath.C_Str() << std::endl;
                    if(texturePathMapping.find(texturePath.C_Str()) == texturePathMapping.end()){
                        header.numTextures++;
                        texturePathMapping[texturePath.C_Str()] = texturePathMapping.size();
                        Asset::TextureHeader textureHeader;
                        textureHeader.width = 0;
                        textureHeader.height = 0;
                        int width, height, nrChannels;
                        stbi_uc* pixels;
                        if (const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                            // Check if the texture is compressed (e.g., PNG or JPEG)
                            if (texture->mHeight == 0) {
                                // Pass compressed data to stb_image
                                pixels = stbi_load_from_memory(
                                    reinterpret_cast<unsigned char*>(texture->pcData),
                                    texture->mWidth, &width, &height, &nrChannels, 4);
                            }
                            else {
                                // Pass uncompressed data to stb_image
                                pixels = stbi_load_from_memory(
                                    reinterpret_cast<unsigned char*>(texture->pcData),
                                    texture->mWidth * texture->mHeight * 4, &width, &height, &nrChannels, 4);
                            }
                        
                        }else{

                            std::string textPath = std::filesystem::path(pathAssimpFile).parent_path().string() + "/" + texturePath.C_Str();
                            std::replace(textPath.begin(), textPath.end(), '\\', '/');
                            pixels = stbi_load
                            (textPath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
                        }

                        if(pixels == NULL)
                        {
                            RX_LOGE("createImage", "stbi_load() failed for", texturePath.C_Str())
                        }

                        strcpy(textureHeader.name, (std::string(texturePath.C_Str())+"_Texture").c_str());
                        textureHeader.width = width;
                        textureHeader.height = height;
                        textureHeader.size = static_cast<uint64_t>(width * height * 4);
                        textureHeaders.push_back(textureHeader);
                        
                        size_t numPixels = static_cast<size_t>(width * height * 4);
                        textureData.insert(textureData.end(), pixels, pixels + numPixels);

                        stbi_image_free(pixels);
                    }

                    materialMapping[i] = texturePathMapping[texturePath.C_Str()];
                }

            }

            uint32_t baseVertex = 0;
            uint32_t baseIndex = 0;

            glm::mat4 rotation(1.f);
            rotation = glm::rotate(rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            for(unsigned int i = 0; i < scene->mNumMeshes; i++){
                aiMesh* aiMesh = scene->mMeshes[i];
                Rx::Component::TextureMaterial textureMaterial;
                aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
                aiColor4D color;
                float value;
                if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &value)) {
                    textureMaterial.metalRough[1] = value;
                }else{
                    textureMaterial.metalRough[1] = 1.0f;
                }
                if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &value)) {
                    textureMaterial.metalRough[0] = value;
                }else{
                    textureMaterial.metalRough[0] = 0.0f;
                }
                if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &color)) {
                    textureMaterial.emissive = glm::vec4(color.r, color.g, color.b, 0.f);
                }else{
                    textureMaterial.emissive = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
                }
                if(materialMapping.find(aiMesh->mMaterialIndex) == materialMapping.end()){
                    textureMaterial.textureIndex[0] = -1;
                    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)){
                        textureMaterial.albedo = glm::vec4(color.r, color.g, color.b, 0.f);
                    }else{
                        textureMaterial.albedo = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
                    }
                }else{
                    textureMaterial.textureIndex[0] = materialMapping[aiMesh->mMaterialIndex];    
                    textureMaterial.albedo = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
                }
                
                materials.push_back(textureMaterial);

                for(unsigned int j = 0; j < aiMesh->mNumVertices; j++){
                    Vertex::Texture vertex;
                    vertex.position = glm::vec3(rotation * glm::vec4(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z, 0.f));
                    vertex.normal = glm::vec3(glm::normalize(rotation * glm::vec4(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z, 0.f)));
                    vertex.texCoord = glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
                    vertexArrays[i].push_back(vertex);
                }

                for(unsigned int j = 0; j < aiMesh->mNumFaces; j++){
                    aiFace face = aiMesh->mFaces[j];
                    RX_ASSERT(face.mNumIndices == 3, "Asset", "convertToTextureModel", "Non-triangle face");
                    indexArrays[i].push_back(face.mIndices[0]);
                    indexArrays[i].push_back(face.mIndices[2]);
                    indexArrays[i].push_back(face.mIndices[1]);
                }
                if(aiMesh->mName.length > 0){
                    strcpy(meshHeaders[i].name, ("Mesh_" + std::to_string(i) + "_" + std::string(aiMesh->mName.C_Str())).c_str());
                }else{
                    strcpy(meshHeaders[i].name, ("Mesh_" + std::to_string(i)).c_str());
                }
                meshHeaders[i].numVertices = vertexArrays[i].size();
                meshHeaders[i].numIndices = indexArrays[i].size();
            }


            std::vector<char> data;
            writeToData(data, &header.numMeshes, 1);
            writeToData(data, &header.numTextures, 1);

            for(uint32_t i = 0; i < materials.size(); i++){
                writeToData(data, &materials[i].textureIndex, 1);
                writeToData(data, &materials[i].albedo, 1);
                writeToData(data, &materials[i].metalRough, 1);
                writeToData(data, &materials[i].emissive, 1);
            }

            for(uint32_t i = 0; i < meshHeaders.size(); i++){
                writeToData(data, &meshHeaders[i].name[0], 64);
                writeToData(data, &meshHeaders[i].numVertices, 1);
                writeToData(data, &meshHeaders[i].numIndices, 1);

                for(uint32_t j = 0; j < vertexArrays[i].size(); j++){
                    writeToData(data, &vertexArrays[i][j].position, 1);
                    writeToData(data, &vertexArrays[i][j].normal, 1);
                    writeToData(data, &vertexArrays[i][j].texCoord, 1);
                }
                writeToData(data, indexArrays[i].data(), indexArrays[i].size());
            }


            for(uint32_t i = 0; i < textureHeaders.size(); i++){
                writeToData(data, &textureHeaders[i].name[0], 64);
                writeToData(data, &textureHeaders[i].size, 1);
                writeToData(data, &textureHeaders[i].width, 1);
                writeToData(data, &textureHeaders[i].height, 1);
            }

            writeToData(data, textureData.data(), textureData.size());

            std::ofstream file(pathTextureModelFile, std::ios::binary);

            RX_ASSERT(file.is_open(), "Asset", "convertToTextureModel",  ("Failed to open file" + pathTextureModelFile).c_str());

            file.write(data.data(), data.size());
            file.close();
        }
    }
}