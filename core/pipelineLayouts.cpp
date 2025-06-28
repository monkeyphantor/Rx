#include "pipelineLayouts.hpp"
#include "descriptorSetLayouts.hpp"
#include "mutex.hpp"
#include "device.hpp"

namespace Rx
{
    namespace Core
    {
        void createPipelineLayouts()
        {
            createColorMeshPipelineLayout();
            createColorMeshArrayPipelineLayout();
        }

        void destroyPipelineLayouts()
        {
            destroyColorMeshPipelineLayout();
            destroyColorMeshArrayPipelineLayout();
        }

        void createColorMeshPipelineLayout(){
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &colorMeshDescriptorSetLayout;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN(
            vkCreatePipelineLayout
            (Core::vkDevice, 
            &layoutInfo, 
            nullptr, 
            &colorMeshPipelineLayout),
            "createPipelineLayout",
            "vkCreatePipelineLayout"
            ));
        }

        void destroyColorMeshPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout(Core::vkDevice, colorMeshPipelineLayout, nullptr);
            );
        }

        void createColorMeshArrayPipelineLayout(){
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &colorMeshArrayDescriptorSetLayout;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN(
            vkCreatePipelineLayout
            (Core::vkDevice, 
            &layoutInfo, 
            nullptr, 
            &colorMeshArrayPipelineLayout),
            "createColorMeshArrayPipelineLayout",
            "vkCreatePipelineLayout"
            ));
        }

        void destroyColorMeshArrayPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout(Core::vkDevice, colorMeshArrayPipelineLayout, nullptr);
            );
        }
    }
} // namespace Rx::Core