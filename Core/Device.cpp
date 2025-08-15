#include "Device.hpp"
#include "PhysicalDevice.hpp"

namespace Rx
{
    namespace Core
    {

        void createDevice()
        {
            float priority = 1.f;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;


            VkPhysicalDeviceVulkan12Features vulkan12Features{};
            vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            vulkan12Features.runtimeDescriptorArray = VK_TRUE;
            vulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
            vulkan12Features.pNext = nullptr;
            
            // Add this when creating the device:
            VkPhysicalDeviceVulkan11Features vulkan11Features{};
            vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            vulkan11Features.shaderDrawParameters = VK_TRUE;
            vulkan11Features.pNext = &vulkan12Features;

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.queueCreateInfoCount = 1;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); 
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();
            createInfo.pNext = &vulkan11Features; // Chain the Vulkan 1.1 features

            RX_CHECK_VULKAN
            (vkCreateDevice
            (vkPhysicalDevice,
            &createInfo,
            nullptr,
            &vkDevice),
            "createDevice",
            "vkCreateDevice")

            #ifdef RX_DEBUG
            RX_LOGI("Device", "created", "")
            #endif
        }

        void destroyDevice()
        {
            vkDestroyDevice
            (vkDevice,
            nullptr);
        }
    }
}