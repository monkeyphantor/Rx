#include "PipelineLayouts.hpp"
#include "DescriptorSetLayouts.hpp"
#include "Mutex.hpp"
#include "Device.hpp"

namespace Rx
{
    namespace Core
    {
        void createPipelineLayouts()
        {
            createColorMeshPipelineLayout();
            createColorMeshArrayPipelineLayout();
            createInstancedColorMeshPipelineLayout();
            createTextureModelPipelineLayout();
            createSkeletonModelPipelineLayout();
            createSkeletonModelCompPipelineLayout();
        }

        void destroyPipelineLayouts()
        {
            destroySkeletonModelCompPipelineLayout();
            destroySkeletonModelPipelineLayout();
            destroyTextureModelPipelineLayout();
            destroyInstancedColorMeshPipelineLayout();
            destroyColorMeshArrayPipelineLayout();
            destroyColorMeshPipelineLayout();
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


        void createInstancedColorMeshPipelineLayout(){
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &instancedColorMeshDescriptorSetLayout;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN(
            vkCreatePipelineLayout
            (Core::vkDevice, 
            &layoutInfo, 
            nullptr, 
            &instancedColorMeshPipelineLayout),
            "createInstancedColorMeshPipelineLayout",
            "vkCreatePipelineLayout"
            ));
        }

        void destroyInstancedColorMeshPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout
            (Core::vkDevice, 
            instancedColorMeshPipelineLayout, 
            nullptr);)
        }

        void createTextureModelPipelineLayout(){
            VkPipelineLayoutCreateInfo layoutCreateInfo{};
            layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutCreateInfo.setLayoutCount = 1;
            layoutCreateInfo.pSetLayouts = &textureModelDescriptorSetLayout;
            layoutCreateInfo.pushConstantRangeCount = 0;
            layoutCreateInfo.pPushConstantRanges = nullptr;

            RX_CHECK_VULKAN
            (vkCreatePipelineLayout
            (Core::vkDevice,
            &layoutCreateInfo,
            nullptr,
            &textureModelPipelineLayout),
            "createTextureModelPipelineLayout",
            "vkCreatePipelineLayout")
        }

        void destroyTextureModelPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout
            (Core::vkDevice, 
            textureModelPipelineLayout, 
            nullptr);)
        }

        void createSkeletonModelPipelineLayout(){
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &skeletonModelDescriptorSetLayout;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN(
            vkCreatePipelineLayout
            (Core::vkDevice, 
            &layoutInfo, 
            nullptr, 
            &skeletonModelPipelineLayout),
            "createSkeletonModelPipelineLayout",
            "vkCreatePipelineLayout"
            ));
        }

        void destroySkeletonModelPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout
            (Core::vkDevice, 
            skeletonModelPipelineLayout, 
            nullptr);
            );
        }

        void createSkeletonModelCompPipelineLayout(){
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = sizeof(int); // As used in the system: struct { int numberNodes; }

            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &skeletonModelCompDescriptorSetLayout;
            layoutInfo.pushConstantRangeCount = 1;
            layoutInfo.pPushConstantRanges = &pushConstantRange;

            RX_VK_MUTEX(
            RX_CHECK_VULKAN(
            vkCreatePipelineLayout
            (Core::vkDevice, 
            &layoutInfo, 
            nullptr, 
            &skeletonModelCompPipelineLayout),
            "createSkeletonModelCompPipelineLayout",
            "vkCreatePipelineLayout"
            ));
        }

        void destroySkeletonModelCompPipelineLayout(){
            RX_VK_MUTEX(
            vkDestroyPipelineLayout(Core::vkDevice, skeletonModelCompPipelineLayout, nullptr);
            );
        }
    }
} // namespace Rx::Core