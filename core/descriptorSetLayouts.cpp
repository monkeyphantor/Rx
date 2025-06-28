#include "descriptorSetLayouts.hpp"
#include "device.hpp"
#include "mutex.hpp"

namespace Rx
{
    namespace Core
    {
        void createDescriptorSetLayouts()
        {
            createColorMeshDescriptorSetLayout();
            createColorMeshArrayDescriptorSetLayout();
        }

        void destroyDescriptorSetLayouts()
        {
            destroyColorMeshDescriptorSetLayout();
            destroyColorMeshArrayDescriptorSetLayout();
        }


        void createColorMeshDescriptorSetLayout()
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings(2);
            bindings[0].binding = 0;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[0].pImmutableSamplers = nullptr;
            bindings[1].binding = 1;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[1].pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            RX_VK_MUTEX(
            RX_CHECK_VULKAN
            (vkCreateDescriptorSetLayout
            (Core::vkDevice,
            &layoutInfo,
            nullptr,
            &colorMeshDescriptorSetLayout),
            "ColorGraphics::createDescriptorSet",
            "vkCreateDescriptorSetLayout"))
            
        }


        void destroyColorMeshDescriptorSetLayout()
        {
            RX_VK_MUTEX(
                vkDestroyDescriptorSetLayout(vkDevice, colorMeshDescriptorSetLayout, nullptr);
            )
        }

        void createColorMeshArrayDescriptorSetLayout()
        {
            // Define descriptor set layout bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings(2);
            bindings[0].binding = 0;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[0].pImmutableSamplers = nullptr;

            bindings[1].binding = 1;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[1].pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            RX_VK_MUTEX(
            RX_CHECK_VULKAN
            (vkCreateDescriptorSetLayout
            (vkDevice, 
            &layoutInfo, 
            nullptr, 
            &colorMeshArrayDescriptorSetLayout),
            "createColorMeshArrayDescriptorSetLayout", 
            "vkCreateDescriptorSetLayout"
            ))
        }

        void destroyColorMeshArrayDescriptorSetLayout()
        {
            RX_VK_MUTEX(
                vkDestroyDescriptorSetLayout(vkDevice, colorMeshArrayDescriptorSetLayout, nullptr);
            )
        }
    }
} // namespace Rx::Core