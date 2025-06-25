#include "allocator.hpp"
#include "instance.hpp"
#include "physicalDevice.hpp"
#include "device.hpp"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

namespace Rx
{
    namespace Core
    {
        void createAllocator()
        {
            VmaAllocatorCreateInfo createInfo{};
            createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
            createInfo.instance = vkInstance;
            createInfo.physicalDevice = vkPhysicalDevice;
            createInfo.device = vkDevice;
            //createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
            createInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
            RX_CHECK_VULKAN
            (vmaCreateAllocator
            (&createInfo,&vmaAllocator),
            "createAllocator",
            "vmaCreateAllocator");

            #ifdef RX_DEBUG
            RX_LOGI("Allocator", "created", "")
            #endif
        }

        void destroyAllocator()
        {
            vmaDestroyAllocator(vmaAllocator);
        }
    }
}