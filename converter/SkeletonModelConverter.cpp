#include "SkeletonModelConverter.hpp"
#include "global.hpp"

#include "vertex.hpp"

#include "Skeleton.hpp"
#include "LoadTextureModel.hpp"
#include "LoadSkeletonModel.hpp"
#include "Material.hpp"
#include "AnimationBone.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssimpHelpers.hpp"

#include "stb/stb_image.h"
#include <filesystem>

namespace Rx
{
    namespace Converter
    {
        void processNode(const aiNode* assimpNode, 
            std::vector<Component::Node>& nodes,
            int& boneIndexCounter, 
            int& nodeIndexCounter,
            std::map<std::string, int>& boneMapping,
            std::map<std::string, int>& nodeMapping,
            const std::map<std::string, glm::mat4>& boneOffsetMap,
            int parentNodeIndex) 
        {
            Component::Node node;
            std::string nodeName(assimpNode->mName.C_Str());
            auto it = boneOffsetMap.find(nodeName);
            if (it != boneOffsetMap.end()) {
                node.isBone = true;
                strcpy(node.name, nodeName.c_str());
                node.offset = it->second;
                boneMapping[nodeName] = boneIndexCounter;
                node.boneIndex = boneIndexCounter;
                boneIndexCounter++;
                nodeMapping[nodeName] = nodeIndexCounter;
                node.nodeIndex = nodeIndexCounter;
                nodeIndexCounter++;
            }else{
                node.isBone = false;
                strcpy(node.name, nodeName.c_str());
                node.offset = AssimpGLMHelpers::convertToMat4(assimpNode->mTransformation);
                node.boneIndex = -1;
                nodeMapping[nodeName] = nodeIndexCounter;
                node.nodeIndex = nodeIndexCounter;
                nodeIndexCounter++;
            }

            int currentIndex = nodes.size()-1;
            for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
                processNode(assimpNode->mChildren[i], nodes, boneIndexCounter, nodeIndexCounter, boneMapping, nodeMapping, boneOffsetMap, currentIndex);
            }
        }

      

        aiNode* findMeshParentNode(aiNode* node, int meshIndex) {
            for (int i = 0; i < node->mNumMeshes; i++) {
                if (node->mMeshes[i] == meshIndex) {
                    return node;
                }
            }
            for (int i = 0; i < node->mNumChildren; i++) {
                aiNode* found = findMeshParentNode(node->mChildren[i], meshIndex);
                if (found != nullptr) {
                    return found;
                }
            }
            return nullptr;
        }

        void convertToSkeletonModelFile(std::string pathAssimpFile, std::string pathTextureModelFile){
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(pathAssimpFile, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);


            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
                RX_LOGE("convertToTextureModel", "Assimp", importer.GetErrorString())
            }


            Asset::MainSkeletonModelHeader header;
            std::vector<Component::TextureMaterial> materials;
            std::vector<Asset::SkeletonMeshHeader> meshHeaders;
            std::vector<std::vector<Rx::Vertex::Skeleton>> vertexArrays;
            std::vector<std::vector<uint32_t>> indexArrays;
            std::vector<Component::Node> nodes;
            std::vector<Asset::AnimationHeader> animationHeaders;
            std::vector<std::vector<Asset::AnimationBoneHeader>> animationBoneHeaders;
            std::vector<std::vector<std::vector<Component::PositionKey>>> positionKeys;
            std::vector<std::vector<std::vector<Component::RotationKey>>> rotationKeys;
            std::vector<std::vector<std::vector<Component::ScalingKey>>> scalingKeys;
            std::vector<Asset::TextureHeader> textureHeaders;
            std::vector<char> textureData;

            header.numberMeshes = scene->mNumMeshes;
            header.numberTextures = 0;
            header.numberAnimations = scene->mNumAnimations;        

            std::map<int32_t /*materialIndex*/, int32_t /*textureIndex*/> materialMapping;
            std::map<std::string /*texture path*/, int32_t /*textureIndex*/> texturePathMapping;

            for(uint32_t i = 0; i < scene->mNumMaterials; i++){
                aiMaterial* material = scene->mMaterials[i];
                aiString texturePath;
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                    std::cout << "Texture path: " << texturePath.C_Str() << std::endl;
                    if(texturePathMapping.find(texturePath.C_Str()) == texturePathMapping.end()){
                        header.numberTextures++;
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

            // Collect bone offset matrices from all meshes
            std::map<std::string, glm::mat4> boneOffsetMap;
            for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
                const aiMesh* mesh = scene->mMeshes[i];
                for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
                    const aiBone* bone = mesh->mBones[j];
                    std::string name(bone->mName.C_Str());
                    if (boneOffsetMap.find(name) == boneOffsetMap.end()) {
                        boneOffsetMap[name] = AssimpGLMHelpers::convertToMat4(bone->mOffsetMatrix);
                    }
                }
            }

            
            std::map<std::string, int> boneMapping;
            std::map<std::string, int> nodeMapping;
            int boneIndexCounter = 0;
            int nodeIndexCounter = 0;
            processNode(scene->mRootNode, nodes, boneIndexCounter, nodeIndexCounter, boneMapping, nodeMapping, boneOffsetMap, -1);
            header.numberNodes = nodes.size();
            header.numberBones = boneIndexCounter;


            uint32_t baseVertex = 0;
            uint32_t baseIndex = 0;        

            for(unsigned int i = 0; i < scene->mNumMeshes; i++){
                std::vector<Vertex::Skeleton> meshVertices;
                aiMesh* aiMesh = scene->mMeshes[i];
                Component::TextureMaterial textureMaterial;
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
                    Vertex::Skeleton vertex;
                    vertex.position = glm::vec3(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
                    vertex.normal = glm::vec3(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
                    vertex.texCoord = glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
                    meshVertices.push_back(vertex);
                }

                if (aiMesh->mNumBones > 0) {
                    // Extract bone information
                    for (unsigned int j = 0; j < aiMesh->mNumBones; j++) {
                        aiBone* bone = aiMesh->mBones[j];
                        std::string boneName(bone->mName.C_Str());

                        if (boneMapping.find(boneName) == boneMapping.end()) {
                            RX_LOGE("convertToSkeletonModelFile", "Bone not found in bone mapping: ", boneName.c_str())
                        }
                        if (nodeMapping.find(boneName) == nodeMapping.end()) {
                            RX_LOGE("convertToSkeletonModeFile", "Bone not found in node mapping: ", boneName.c_str());
                        }
                        
                        int nodeIndex;
                        nodeIndex = nodeMapping[boneName];

                        for (unsigned int k = 0; k < bone->mNumWeights; k++) {
                            aiVertexWeight weight = bone->mWeights[k];
                            int vertexID = weight.mVertexId;
                            float boneWeight = weight.mWeight;

                            Vertex::Skeleton& vertex = meshVertices[vertexID];
                            for (int l = 0; l < 4; l++) {
                                if (vertex.weights[l] == 0.0f) {
                                    vertex.bones[l] = nodeIndex;
                                    vertex.weights[l] = boneWeight;
                                    break;
                                }
                            }
                        }
                    }
                }
                else {
                    aiNode* parentNode = findMeshParentNode(scene->mRootNode, i);

                    if (parentNode == nullptr) {
                        RX_LOGE("convertToSkeletonModelFile", "Parent node not found for mesh: ", std::to_string(i).c_str());
                    }

                    std::string nodeName(parentNode->mName.C_Str());
                    if (nodeMapping.find(nodeName) == nodeMapping.end()) {
                        RX_LOGE("convertToSkeletonModelFile", "Node not found in node mapping: ", nodeName.c_str());
                    }

                    int nodeIndex = nodeMapping[nodeName];

                    for (int j = 0; j < aiMesh->mNumVertices; j++) {
                        meshVertices[j].nodeIndex = nodeIndex;
                    }
                }

                vertexArrays.push_back(std::move(meshVertices));

                std::vector<uint32_t> indices;
                for(unsigned int j = 0; j < aiMesh->mNumFaces; j++){
                    aiFace face = aiMesh->mFaces[j];
                    RX_ASSERT(face.mNumIndices == 3, "Asset", "convertToTextureModel", "Non-triangle face");
                    indices.push_back(face.mIndices[0]);
                    indices.push_back(face.mIndices[2]);
                    indices.push_back(face.mIndices[1]);
                }
                indexArrays.push_back(std::move(indices));

                Asset::SkeletonMeshHeader meshHeader;
                if(aiMesh->mName.length > 0){
                    strcpy(meshHeader.name, ("Mesh_" + std::to_string(i) + "_" + std::string(aiMesh->mName.C_Str())).c_str());
                }else{
                    strcpy(meshHeader.name, ("Mesh_" + std::to_string(i)).c_str());
                }
                meshHeader.numVertices = vertexArrays[i].size();
                meshHeader.numIndices = indexArrays[i].size();
                meshHeaders.push_back(meshHeader);
            }
            
            
            for(uint32_t i = 0; i < scene->mNumAnimations; i++){
                aiAnimation* animation = scene->mAnimations[i];
                Asset::AnimationHeader animationHeader;
                strcpy(animationHeader.name, animation->mName.C_Str());
                animationHeader.duration = animation->mDuration;
                if(animation->mTicksPerSecond){
                    animationHeader.ticksPerSecond = animation->mTicksPerSecond;
                }else{
                }
                animationHeaders.push_back(animationHeader);

                std::cout << "Animation: " << animation->mName.C_Str() << " Duration: " << animation->mDuration << std::endl;

                animationBoneHeaders.push_back(std::vector<Asset::AnimationBoneHeader>(boneMapping.size()));
                positionKeys.push_back(std::vector<std::vector<Component::PositionKey>>(boneMapping.size()));
                rotationKeys.push_back(std::vector<std::vector<Component::RotationKey>>(boneMapping.size()));
                scalingKeys.push_back(std::vector<std::vector<Component::ScalingKey>>(boneMapping.size()));

                auto tempBoneMap = boneMapping;


                for(uint32_t j = 0; j < animation->mNumChannels; j++){
                    aiNodeAnim* nodeAnim = animation->mChannels[j];
                    if (boneMapping.find(nodeAnim->mNodeName.C_Str()) == boneMapping.end()) {
                        RX_LOGI("Asset", "convertToTextureModel", ("Bone of Channel: " + std::string(nodeAnim->mNodeName.C_Str()) + "not found").c_str());
                        continue; // Skip this channel if the bone is not found
                    }
                    Asset::AnimationBoneHeader animationBoneHeader;
                    strcpy(animationBoneHeader.name, nodeAnim->mNodeName.C_Str());
                    animationBoneHeader.numPositionKeys = nodeAnim->mNumPositionKeys;
                    animationBoneHeader.numRotationKeys = nodeAnim->mNumRotationKeys;
                    animationBoneHeader.numScalingKeys = nodeAnim->mNumScalingKeys;
                    animationBoneHeaders[i][boneMapping[nodeAnim->mNodeName.C_Str()]] = animationBoneHeader;

                    std::vector<Component::PositionKey> positionKey;
                    std::vector<Component::RotationKey> rotationKey;
                    std::vector<Component::ScalingKey> scalingKey;

                    for(uint32_t k = 0; k < nodeAnim->mNumPositionKeys; k++){
                        Component::PositionKey key;

                        key.time = nodeAnim->mPositionKeys[k].mTime;
                        key.position = AssimpGLMHelpers::convertToVec3(nodeAnim->mPositionKeys[k].mValue);
                        positionKey.push_back(key);
                    }

                    for(uint32_t k = 0; k < nodeAnim->mNumRotationKeys; k++){
                        Component::RotationKey key;
                        key.time = nodeAnim->mRotationKeys[k].mTime;
                        key.rotation = AssimpGLMHelpers::convertToQuat(nodeAnim->mRotationKeys[k].mValue);
                        rotationKey.push_back(key);
                    }

                    for(uint32_t k = 0; k < nodeAnim->mNumScalingKeys; k++){
                        Component::ScalingKey key;
                        key.time = nodeAnim->mScalingKeys[k].mTime;
                        key.scaling = AssimpGLMHelpers::convertToVec3(nodeAnim->mScalingKeys[k].mValue);
                        scalingKey.push_back(key);
                    }

                    positionKeys[i][boneMapping[nodeAnim->mNodeName.C_Str()]] = positionKey;
                    rotationKeys[i][boneMapping[nodeAnim->mNodeName.C_Str()]] = rotationKey;
                    scalingKeys[i][boneMapping[nodeAnim->mNodeName.C_Str()]] = scalingKey;

                    tempBoneMap.erase(nodeAnim->mNodeName.C_Str());
                }

                for (const auto [name, index] : tempBoneMap) {
                    // pair.first is the bone name, pair.second is the bone index
                    Asset::AnimationBoneHeader animBoneHeader;
                    strcpy(animBoneHeader.name, name.c_str());
                    animBoneHeader.numPositionKeys = 1;
                    animBoneHeader.numRotationKeys = 1;
                    animBoneHeader.numScalingKeys  = 1;
                    animationBoneHeaders[i][index] = animBoneHeader;

                    // Create identity keyframes
                    Component::PositionKey posKey;
                    posKey.time = 0.0f;
                    posKey.position = glm::vec3(0.0f); // Identity position (no translation)

                    Component::RotationKey rotKey;
                    rotKey.time = 0.0f;
                    rotKey.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion

                    Component::ScalingKey scaleKey;
                    scaleKey.time = 0.0f;
                    scaleKey.scaling = glm::vec3(1.0f); // Identity scaling

                    // Store them in the respective buffers at this bone index
                    positionKeys[i][index] = std::vector<Component::PositionKey>{ posKey };
                    rotationKeys[i][index] = std::vector<Component::RotationKey>{ rotKey };
                    scalingKeys[i][index] = std::vector<Component::ScalingKey>{ scaleKey };
                }

            }

            std::vector<char> data;
            writeToData(data, &header.numberMeshes, 1);
            writeToData(data, &header.numberNodes, 1);
            writeToData(data, &header.numberBones, 1);
            writeToData(data, &header.numberAnimations, 1);
            writeToData(data, &header.numberTextures, 1);
            
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
                    writeToData(data, &vertexArrays[i][j].bones, 1);
                    writeToData(data, &vertexArrays[i][j].weights, 1);
                    writeToData(data, &vertexArrays[i][j].nodeIndex, 1);
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

            for(uint32_t i = 0; i < nodes.size(); i++){
                writeToData(data, &nodes[i].isBone, 1);
                writeToData(data, &nodes[i].name[0], 128);
                writeToData(data, &nodes[i].offset, 1);
                writeToData(data, &nodes[i].numberChildren, 1);
                writeToData(data, &nodes[i].nodeIndex, 1);
                writeToData(data, &nodes[i].boneIndex, 1);
            }

            for(uint32_t i = 0; i < animationHeaders.size(); i++){
                writeToData(data, &animationHeaders[i].name[0], 128);
                writeToData(data, &animationHeaders[i].duration, 1);
                writeToData(data, &animationHeaders[i].ticksPerSecond, 1);
                
                for(uint32_t j = 0; j < animationBoneHeaders[i].size(); j++){
                    writeToData(data, &animationBoneHeaders[i][j].name[0], 128);
                    writeToData(data, &animationBoneHeaders[i][j].numPositionKeys, 1);
                    writeToData(data, &animationBoneHeaders[i][j].numRotationKeys, 1);
                    writeToData(data, &animationBoneHeaders[i][j].numScalingKeys, 1);

                    for(uint32_t k = 0; k < positionKeys[i][j].size(); k++){
                        writeToData(data, &positionKeys[i][j][k].time, 1);
                        writeToData(data, &positionKeys[i][j][k].position, 1);
                    }

                    for(uint32_t k = 0; k < rotationKeys[i][j].size(); k++){
                        writeToData(data, &rotationKeys[i][j][k].time, 1);
                        writeToData(data, &rotationKeys[i][j][k].rotation, 1);
                    }

                    for(uint32_t k = 0; k < scalingKeys[i][j].size(); k++){
                        writeToData(data, &scalingKeys[i][j][k].time, 1);
                        writeToData(data, &scalingKeys[i][j][k].scaling, 1);
                    }
                }
            }

            std::ofstream file(pathTextureModelFile, std::ios::binary);

            RX_ASSERT(file.is_open(), "Asset", "convertToTextureModel",  ("Failed to open file" + pathTextureModelFile).c_str());

            file.write(data.data(), data.size());
            file.close();
        }

    }
}
