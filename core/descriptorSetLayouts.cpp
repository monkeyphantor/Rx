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
            createInstancedColorMeshDescriptorSetLayout();
        }

        void destroyDescriptorSetLayouts()
        {
            destroyInstancedColorMeshDescriptorSetLayout();
            destroyColorMeshArrayDescriptorSetLayout();
            destroyColorMeshDescriptorSetLayout();
        }


        void createColorMeshDescriptorSetLayout()
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings(4);
            bindings[0].binding = 0;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[0].pImmutableSamplers = nullptr;
            bindings[1].binding = 1;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[1].pImmutableSamplers = nullptr;
            bindings[2].binding = 2;
            bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[2].descriptorCount = 1;
            bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[2].pImmutableSamplers = nullptr;
            bindings[3].binding = 3;
            bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[3].descriptorCount = 1;
            bindings[3].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[3].pImmutableSamplers = nullptr;

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
            "VkColorModelDescriptorSet::createDescriptorSet",
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
            std::vector<VkDescriptorSetLayoutBinding> bindings(4);
            bindings[0].binding = 0;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[0].pImmutableSamplers = nullptr;
            bindings[1].binding = 1;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[1].pImmutableSamplers = nullptr;
            bindings[2].binding = 2;
            bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[2].descriptorCount = 1;
            bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[2].pImmutableSamplers = nullptr;
            bindings[3].binding = 3;
            bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            bindings[3].descriptorCount = 1;
            bindings[3].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[3].pImmutableSamplers = nullptr;

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



        void createInstancedColorMeshDescriptorSetLayout()
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings(4);
            bindings[0].binding = 0;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[0].pImmutableSamplers = nullptr;
            bindings[1].binding = 1;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[1].pImmutableSamplers = nullptr;
            bindings[2].binding = 2;
            bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[2].descriptorCount = 1;
            bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[2].pImmutableSamplers = nullptr;
            bindings[3].binding = 3;
            bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            bindings[3].descriptorCount = 1;
            bindings[3].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings[3].pImmutableSamplers = nullptr;

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
            &instancedColorMeshDescriptorSetLayout),
            "createInstancedColorMeshDescriptorSetLayout", 
            "vkCreateDescriptorSetLayout"))
        }

        void destroyInstancedColorMeshDescriptorSetLayout()
        {
            RX_VK_MUTEX(
            vkDestroyDescriptorSetLayout
            (vkDevice, 
            instancedColorMeshDescriptorSetLayout,
            nullptr);)
        }
    }
} // namespace Rx::Core