#include "colorArrayGraphics.hpp"
#include "../core/descriptorPool.hpp"
#include "../core/descriptorSetLayouts.hpp"
#include "../core/mutex.hpp"
#include "../core/eye.hpp"
#include "../core/device.hpp"
#include "colorMeshInstance.hpp"
namespace Rx
{
    namespace Component
    {
        void colorArrayGraphics_component_on_add(flecs::entity e, ColorMeshInstanceBuffer& instances, ColorArrayGraphics& graphics){
            // Create descriptor set for the graphics component
            graphics.createDescriptorSet(instances.instanceBuffer);

        }
        
        void colorArrayGraphics_component_on_remove(flecs::entity e, ColorArrayGraphics& graphics){
            // Destroy the descriptor set when the component is removed
            graphics.destroyDescriptorSet();
        }


        void ColorArrayGraphics::createDescriptorSet(Core::BufferInterface& instanceBuffer){
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = Core::vkDescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &Core::colorMeshArrayDescriptorSetLayout;
            
            RX_VK_MUTEX(
            RX_CHECK_VULKAN
            (vkAllocateDescriptorSets
            (Core::vkDevice,
            &allocInfo,
            &vkDescriptorSet),
            "ColorArrayGraphics::createDescriptorSet",
            "vkAllocateDescriptorSets"))

            std::vector<VkDescriptorBufferInfo> bufferInfo(2);
            bufferInfo[0].buffer = Core::eye.vkBuffer;
            bufferInfo[0].offset = 0;
            bufferInfo[0].range = sizeof(Core::Eye);
            bufferInfo[1].buffer = instanceBuffer.vkBuffer;
            bufferInfo[1].offset = 0;
            bufferInfo[1].range = sizeof(ColorMeshInstance) * instanceBuffer.numberElements;
            
            std::vector<VkWriteDescriptorSet> writeSet(2);
            writeSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet[0].dstSet = vkDescriptorSet;
            writeSet[0].dstBinding = 0;
            writeSet[0].dstArrayElement = 0;
            writeSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
            
            RX_VK_MUTEX(
            vkUpdateDescriptorSets
            (Core::vkDevice, 
            static_cast<uint32_t>(writeSet.size()),
            writeSet.data(),
            0, nullptr));
        }

        void ColorArrayGraphics::destroyDescriptorSet(){
            RX_VK_MUTEX(
            vkFreeDescriptorSets
            (Core::vkDevice, 
            Core::vkDescriptorPool, 
            1, 
            &vkDescriptorSet);)
            vkDescriptorSet = VK_NULL_HANDLE;
        }
    }
}
