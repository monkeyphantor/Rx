#include "VkSkeletonModelCompDescriptorSet.hpp"
#include "mutex.hpp"
#include "device.hpp"
#include "descriptorPool.hpp"
#include "descriptorSetLayouts.hpp"
#include "NodeTransform.hpp"

namespace Rx{
namespace Component{
    void vkSkeletonModelCompDescriptorSet_component_on_add(flecs::entity e, VkSkeleton& vkSkeleton, VkKeyFrameArrayBuffer& keyFrameBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer, VkSkeletonModelCompDescriptorSet& descriptorSet){
        descriptorSet.createDescriptorSet(vkSkeleton, keyFrameBuffer, skeletonArrayBuffer);
    }

    void vkSkeletonModelCompDescriptorSet_component_on_remove(flecs::entity e, VkSkeletonModelCompDescriptorSet& descriptorSet){
        descriptorSet.destroyDescriptorSet();
    }

    void VkSkeletonModelCompDescriptorSet::createDescriptorSet(VkSkeleton& vkSkeleton, VkKeyFrameArrayBuffer& keyFrameBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer){
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = Core::vkDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &Core::skeletonModelCompDescriptorSetLayout;

        RX_VK_MUTEX(
        RX_CHECK_VULKAN(
            vkAllocateDescriptorSets(Core::vkDevice, &allocInfo, &vkDescriptorSet),
            "VkSkeletonModelCompDescriptorSet::createDescriptorSet",
            "vkAllocateDescriptorSets"))

        std::vector<VkDescriptorBufferInfo> bufferInfo(3);
        bufferInfo[0].buffer = vkSkeleton.buffer.vkBuffer;
        bufferInfo[0].offset = 0;
        bufferInfo[0].range = sizeof(VkSkeletonNode) * vkSkeleton.nodeCount;
        bufferInfo[1].buffer = keyFrameBuffer.buffer.vkBuffer;
        bufferInfo[1].offset = 0;
        bufferInfo[1].range = sizeof(VkKeyFrame) * keyFrameBuffer.maxNumberKeyFrameArrays * 256;
        bufferInfo[2].buffer = skeletonArrayBuffer.buffer.vkBuffer;
        bufferInfo[2].offset = 0;
        bufferInfo[2].range = sizeof(NodeTransform) * skeletonArrayBuffer.maxNumberSkeletons * 256;

        std::vector<VkWriteDescriptorSet> writeSet(3);
        for(size_t i=0;i<writeSet.size();++i){
            writeSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet[i].dstSet = vkDescriptorSet;
            writeSet[i].dstArrayElement = 0;
            writeSet[i].descriptorCount = 1;
            writeSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeSet[i].pImageInfo = nullptr;
            writeSet[i].pTexelBufferView = nullptr;
            writeSet[i].pNext = nullptr;
        }
        writeSet[0].dstBinding = 0; writeSet[0].pBufferInfo = &bufferInfo[0];
        writeSet[1].dstBinding = 1; writeSet[1].pBufferInfo = &bufferInfo[1];
        writeSet[2].dstBinding = 2; writeSet[2].pBufferInfo = &bufferInfo[2];

        RX_VK_MUTEX(vkUpdateDescriptorSets(Core::vkDevice, static_cast<uint32_t>(writeSet.size()), writeSet.data(), 0, nullptr));
    }

    void VkSkeletonModelCompDescriptorSet::destroyDescriptorSet(){
        vkFreeDescriptorSets(Core::vkDevice, Core::vkDescriptorPool, 1, &vkDescriptorSet);
    }
}
}
