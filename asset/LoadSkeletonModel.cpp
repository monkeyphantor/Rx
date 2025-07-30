#include "LoadSkeletonModel.hpp"
#include "LoadDataHelper.hpp"
#include "SkeletonMeshArray.hpp"
#include "TextureMaterialBuffer.hpp"
#include "IndirectBuffer.hpp"
#include "TextureArray.hpp"
#include "Material.hpp"
#include "VkSkeletonMeshArray.hpp"
#include "VkTextureMaterialBuffer.hpp"
#include "VkIndirectBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureArray.hpp"
#include "VkSkeletonModelDescriptorSet.hpp"
#include "gameWorldBase.hpp"
#include "LoadTextureModel.hpp"
#include "NodeTransform.hpp"
#include "Node.hpp"

#include "AnimationBone.hpp"
#include "AnimationClip.hpp"
#include "AnimationMap.hpp"
#include "Skeleton.hpp"
#include "SkeletonBuffer.hpp"
#include "LocalTransform.hpp"
#include "SkeletonInstance.hpp"
#include "NodeChildren.hpp"

namespace Rx {

namespace Asset{

     void createNodePrefabs(flecs::world& world, const std::vector<Component::Node>& nodes, std::vector<flecs::entity>& nodePrefabs, const std::string& assetName, const flecs::entity& parent, std::vector<flecs::entity>& animationClipEntities, std::vector<std::vector<std::vector<Component::PositionKey>>> positionKeys, std::vector<std::vector<std::vector<Component::RotationKey>>> rotationKeys, std::vector<std::vector<std::vector<Component::ScalingKey>>> scalingKeys) {

            flecs::entity nodePrefab = world.prefab((assetName + "_Node_" + nodes[nodePrefabs.size()].name).c_str());
            uint32_t nodeIndex = static_cast<uint32_t>(nodePrefabs.size());
            nodePrefabs.push_back(nodePrefab);
            nodePrefab.set<Rx::Component::NodeIndex>({nodeIndex});
            if(nodes[nodeIndex].isBone){
                nodePrefab.set<Rx::Component::BoneIndex>({static_cast<uint32_t>(nodes[nodeIndex].boneIndex)});
                nodePrefab.set<Rx::Component::InvBindPose>({nodes[nodeIndex].offset});
                for(uint32_t i = 0; i < animationClipEntities.size(); i++){
                    Rx::Component::AnimationBone bone(
                        nodes[nodeIndex].name,
                        std::move(positionKeys[i][nodes[nodeIndex].boneIndex]),
                        std::move(rotationKeys[i][nodes[nodeIndex].boneIndex]),
                        std::move(scalingKeys[i][nodes[nodeIndex].boneIndex])
                    );
                    flecs::entity animationClipEntity = animationClipEntities[i];
                    nodePrefab.set<Rx::Component::AnimationBone>(animationClipEntity, std::move(bone));
                }
            }else{
                Rx::Component::LocalTransform localTransform;
                localTransform.fromGlmMat4(nodes[nodeIndex].offset);
                nodePrefab.set<Rx::Component::LocalTransform>(localTransform);
            }

            for(int j = 0; j < nodes[nodeIndex].numberChildren; j++){
                createNodePrefabs(world, nodes, nodePrefabs, assetName, nodePrefab, animationClipEntities, positionKeys, rotationKeys, scalingKeys);
            }
        }



    flecs::entity /*assetEntity*/ loadSkeletonModel(const std::string& filePath, flecs::world& world, const std::string& assetName, const uint32_t maxNumberInstances) {

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

        auto asset = world.entity(assetName.c_str());
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

        for(uint32_t i = 0; i < nodes.size(); i++){
            loadData(data, nodes[i].isBone, offset);
            loadString(data, nodes[i].name, 128, offset);
            loadData(data, nodes[i].offset, offset);
            loadData(data, nodes[i].numberChildren, offset);
            for(uint32_t j = 0; j < 32; j++){
               loadData(data, nodes[i].children[j], offset);
            }
			loadData(data, nodes[i].nodeIndex, offset);
            loadData(data, nodes[i].boneIndex, offset);
        }


        std::vector<AnimationHeader> animationHeaders(header.numberAnimations);
        std::vector<std::vector<AnimationBoneHeader>> animationBoneHeaders(header.numberAnimations);
        std::vector<std::vector<std::vector<Component::PositionKey>>> positionKeys(header.numberAnimations);
        std::vector<std::vector<std::vector<Component::RotationKey>>> rotationKeys(header.numberAnimations);
        std::vector<std::vector<std::vector<Component::ScalingKey>>> scalingKeys(header.numberAnimations);

        for(uint32_t i = 0; i < animationHeaders.size(); i++){
            loadString(data, &animationHeaders[i].name[0], 128, offset);
            loadData(data, animationHeaders[i].duration, offset);
            loadData(data, animationHeaders[i].ticksPerSecond, offset);

            animationBoneHeaders[i].resize(header.numberBones);
            positionKeys[i].resize(header.numberBones);
            rotationKeys[i].resize(header.numberBones);
            scalingKeys[i].resize(header.numberBones);    
            for(uint32_t j = 0; j < animationBoneHeaders[i].size(); j++){
                loadString(data, &animationBoneHeaders[i][j].name[0], 128, offset);
                loadData(data, animationBoneHeaders[i][j].numPositionKeys, offset);
                loadData(data, animationBoneHeaders[i][j].numRotationKeys, offset);
                loadData(data, animationBoneHeaders[i][j].numScalingKeys, offset);

                positionKeys[i][j].resize(animationBoneHeaders[i][j].numPositionKeys);
                for(uint32_t k = 0; k < positionKeys[i][j].size(); k++){
                    loadData(data, positionKeys[i][j][k].time, offset);
                    loadData(data, positionKeys[i][j][k].position, offset);
                }

                rotationKeys[i][j].resize(animationBoneHeaders[i][j].numRotationKeys);
                for(uint32_t k = 0; k < rotationKeys[i][j].size(); k++){
                    loadData(data, rotationKeys[i][j][k].time, offset);
                    loadData(data, rotationKeys[i][j][k].rotation, offset);
                }

                scalingKeys[i][j].resize(animationBoneHeaders[i][j].numScalingKeys);
                for(uint32_t k = 0; k < scalingKeys[i][j].size(); k++){
                    loadData(data, scalingKeys[i][j][k].time, offset);
                    loadData(data, scalingKeys[i][j][k].scaling, offset);
                }
            }
        }       

        asset.set<Rx::Component::SkeletonMeshArray>(meshArray);
        asset.set<Rx::Component::TextureMaterialBuffer>(textureMaterialBuffer);
        asset.set<Rx::Component::IndirectBuffer>(indirectBuffer);
        asset.set<Rx::Component::TextureArray>(textureArray);

        asset.add<Rx::Component::VkSkeletonMeshArray>();
        asset.add<Rx::Component::VkTextureMaterialBuffer>();
        
        Rx::Component::VkIndirectBuffer vkIndirectBuffer;
        vkIndirectBuffer.maxNumberCommands = header.numberMeshes;
        vkIndirectBuffer.numberCommands = 0;
        asset.set<Rx::Component::VkIndirectBuffer>(vkIndirectBuffer);

        asset.add<Rx::Component::VkTextureArray>();

        Rx::Component::VkTransformBuffer transformBuffer;
        transformBuffer.maxNumberTransforms = maxNumberInstances;
        transformBuffer.numberTransforms = 0;
        asset.set<Rx::Component::VkTransformBuffer>(transformBuffer);

        Rx::Component::VkSkeletonArrayBuffer vkSkeletonArrayBuffer;
        vkSkeletonArrayBuffer.maxNumberSkeletons = maxNumberInstances;
        vkSkeletonArrayBuffer.numberSkeletons = 0;
        asset.set<Rx::Component::VkSkeletonArrayBuffer>(vkSkeletonArrayBuffer);

        
        asset.add<Rx::Component::VkSkeletonModelDescriptorSet>();
        asset.add<Rx::ShouldBeUpdated>();

        flecs::entity animationPrefab = world.prefab((assetName + "_Animation").c_str());

        Rx::Component::AnimationMap map;
        std::vector<flecs::entity> animationClipEntities;
        for(uint32_t i = 0; i < header.numberAnimations; i++){
            flecs::entity animationClipEntity = world.entity((assetName + "_AnimationClip_" + animationHeaders[i].name).c_str());
            Component::AnimationClip animationClip;
            animationClip.duration = animationHeaders[i].duration;
            animationClip.ticksPerSecond = animationHeaders[i].ticksPerSecond;
            animationClip.durationInSeconds = animationHeaders[i].duration / animationHeaders[i].ticksPerSecond;
            animationPrefab.set<Rx::Component::AnimationClip>(animationClipEntity, animationClip);
            animationClipEntities.push_back(animationClipEntity);
            map.animations[animationHeaders[i].name] = animationClipEntity;
        }
        animationPrefab.set<Rx::Component::AnimationMap>({map});

        flecs::entity rootNodeEntity = world.entity((assetName + "_RootNode_" + nodes[0].name).c_str());
        rootNodeEntity.set<Rx::Component::NodeIndex>({0});
        if(nodes[0].isBone){
            rootNodeEntity.set<Rx::Component::BoneIndex>({0});
            rootNodeEntity.set<Rx::Component::InvBindPose>({nodes[0].offset});
            for(uint32_t i = 0; i < animationClipEntities.size(); i++){
                 // Explicitly create the AnimationBone component first
                Rx::Component::AnimationBone bone(
                    nodes[0].name,
                    std::move(positionKeys[i][0]),
                    std::move(rotationKeys[i][0]),
                    std::move(scalingKeys[i][0])
                );
                flecs::entity animationClipEntity = animationClipEntities[i];
                rootNodeEntity.set<Rx::Component::AnimationBone>(animationClipEntity, std::move(bone));
            }
        }else{
            Rx::Component::LocalTransform localTransform;
            localTransform.fromGlmMat4(nodes[0].offset);
            rootNodeEntity.set<Rx::Component::LocalTransform>(localTransform);
        }


        std::vector<flecs::entity> nodePrefabs;
        nodePrefabs.push_back(rootNodeEntity);
		for (uint32_t i = 0; i < nodes[0].numberChildren; i++) {
			createNodePrefabs(world, nodes, nodePrefabs, assetName, rootNodeEntity, animationClipEntities, positionKeys, rotationKeys, scalingKeys);
		}

        asset.set<Rx::Component::Skeleton>({nodes, nodePrefabs, animationPrefab});

        return asset;
    }




    void createNodeInstanceEntities(flecs::world& world, const std::string& skelInstanceName, const Component::Skeleton& skeleton, std::vector<flecs::entity>& nodeEntities, const flecs::entity& parent) {

        uint32_t nodeIndex = static_cast<uint32_t>(nodeEntities.size());
        flecs::entity nodeInstanceEntity = world.entity((std::string(skeleton.nodePrefabs[nodeIndex].name().c_str()) + "_Instance_of_" + skelInstanceName).c_str());
        nodeEntities.push_back(nodeInstanceEntity);
        nodeInstanceEntity.is_a(skeleton.nodePrefabs[nodeIndex]);
        nodeInstanceEntity.add(world.lookup("IsChildNodeOf"), parent);
        Rx::Component::NodeChildren nodeChildren;
        nodeChildren.numberChildren = skeleton.nodes[nodeIndex].numberChildren;
        nodeInstanceEntity.set<Rx::Component::NodeChildren>(nodeChildren);

        for(int j = 0; j < skeleton.nodes[nodeIndex].numberChildren; j++){
            createNodeInstanceEntities(world, skelInstanceName, skeleton, nodeEntities, nodeInstanceEntity);
        }
    }


    flecs::entity createSkeletonModelInstance(flecs::world& world, const flecs::entity& asset, const std::string& name)
    {
        flecs::entity instanceEntity = world.entity(name.c_str());
        instanceEntity.add(world.lookup("IsSkeletonModelInstanceOf"), asset);
        
        auto& skeleton = asset.get<Rx::Component::Skeleton>();
        Component::SkeletonBuffer skeletonBuffer;
        skeletonBuffer.transforms.resize(skeleton.nodes.size());
        for(uint32_t i = 0; i < skeletonBuffer.transforms.size(); i++ ){
            skeletonBuffer.transforms[i] = {glm::mat4(1.f), glm::mat4(1.f)};
        }
        instanceEntity.set<Rx::Component::SkeletonBuffer>(skeletonBuffer);


        flecs::entity animationPrefab = skeleton.animationPrefab;
        flecs::entity animationInstanceEntity = world.entity((std::string(animationPrefab.name().c_str()) + "_Instance_of_" + name).c_str());
        animationInstanceEntity.is_a(animationPrefab);
        animationInstanceEntity.add(world.lookup("IsAnimationOf"), instanceEntity);
        
        flecs::entity rootNodeEntityPrefab = skeleton.nodePrefabs[0];

        flecs::entity rootNodeEntity = world.entity((std::string(rootNodeEntityPrefab.name().c_str()) + "_Instance_of_" + name).c_str());
        rootNodeEntity.is_a(rootNodeEntityPrefab);
        rootNodeEntity.add(world.lookup("IsRootNodeOf"), animationInstanceEntity);
        Rx::Component::NodeChildren rootNodeChildren;
        rootNodeChildren.numberChildren = skeleton.nodes[0].numberChildren;
        rootNodeEntity.set<Rx::Component::NodeChildren>(rootNodeChildren);

        std::vector<flecs::entity> nodeInstanceEntities;
        nodeInstanceEntities.push_back(rootNodeEntity);

        for (int j = 0; j < skeleton.nodes[0].numberChildren; j++) {
            createNodeInstanceEntities(world, name, skeleton, nodeInstanceEntities, rootNodeEntity);
        }

        instanceEntity.set<Rx::Component::SkeletonInstance>({nodeInstanceEntities});

        return instanceEntity;
    }

}
}