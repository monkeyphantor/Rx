#include "VkSkeletonModelCompDescriptorSet.hpp"
#include "descriptorPool.hpp"
#include "mutex.hpp"
#include "device.hpp"
#include "NodeTransform.hpp"
#include "descriptorSetLayouts.hpp"

namespace Rx{
    namespace Component{

    



        void vkSkeletonModelCompDescriptorSet_component_on_add(flecs::entity e, VkSkeleton& skeleton, VkKeyFrameArrayBuffer& keyFrameArrayBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer, VkSkeletonModelCompDescriptorSet& descriptorSet){
            descriptorSet.createDescriptorSet(skeleton, keyFrameArrayBuffer, skeletonArrayBuffer);
        }


        void vkSkeletonModelCompDescriptorSet_component_on_remove(flecs::entity e, VkSkeletonModelCompDescriptorSet& descriptorSet){
            descriptorSet.destroyDescriptorSet();
        }


        void VkSkeletonModelCompDescriptorSet::createDescriptorSet(VkSkeleton& skeleton, VkKeyFrameArrayBuffer& keyFrameArrayBuffer, VkSkeletonArrayBuffer& skeletonArrayBuffer){
             VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = Core::vkDescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &Core::skeletonModelCompDescriptorSetLayout;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN
            (vkAllocateDescriptorSets
            (Core::vkDevice,
            &allocInfo,
            &vkDescriptorSet),
            "VkSkeletonModelDescriptorSet::createDescriptorSet",
            "vkAllocateDescriptorSets"))

            std::vector<VkDescriptorBufferInfo> bufferInfo(3);
            bufferInfo[0].buffer = skeleton.skeleton.vkBuffer;
            bufferInfo[0].offset = 0;
            bufferInfo[0].range = sizeof(VkNode) * 256;
            bufferInfo[1].buffer = keyFrameArrayBuffer.buffer.vkBuffer;
            bufferInfo[1].offset = 0;
            bufferInfo[1].range = sizeof(VkKeyFrame) * keyFrameArrayBuffer.maxNumberKeyFrameArrays * 256;
            bufferInfo[2].buffer = skeletonArrayBuffer.buffer.vkBuffer;
            bufferInfo[2].offset = 0;
            bufferInfo[2].range = sizeof(NodeTransform) * skeletonArrayBuffer.maxNumberSkeletons * 256;
            
            std::vector<VkWriteDescriptorSet> writeSet(3);
            writeSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet[0].dstSet = vkDescriptorSet;
            writeSet[0].dstBinding = 0;
            writeSet[0].dstArrayElement = 0;
            writeSet[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeSet[0].descriptorCount = 1;
            writeSet[0].pBufferInfo = &bufferInfo[0];
            writeSet[0].pImageInfo = nullptr;
            writeSet[0].pTexelBufferView = nullptr;
            writeSet[0].pNext = nullptr;
            writeSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet[1].dstSet = vkDescriptorSet;
            writeSet[1].dstBinding = 1;
            writeSet[1].dstArrayElement = 0;
            writeSet[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeSet[1].descriptorCount = 1;
            writeSet[1].pBufferInfo = &bufferInfo[1];
            writeSet[1].pImageInfo = nullptr;
            writeSet[1].pTexelBufferView = nullptr;
            writeSet[1].pNext = nullptr;
            writeSet[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet[2].dstSet = vkDescriptorSet;
            writeSet[2].dstBinding = 2;
            writeSet[2].dstArrayElement = 0;
            writeSet[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeSet[2].descriptorCount = 1;
            writeSet[2].pBufferInfo = &bufferInfo[2];
            writeSet[2].pImageInfo = nullptr;
            writeSet[2].pTexelBufferView = nullptr;
            writeSet[2].pNext = nullptr;
            
            RX_VK_MUTEX(
            vkUpdateDescriptorSets
            (Core::vkDevice, 
            static_cast<uint32_t>(writeSet.size()),
            writeSet.data(),
            0, nullptr));
 }

        void VkSkeletonModelCompDescriptorSet::destroyDescriptorSet(){
            // Implementation for destroying the descriptor set
        }
}
}