#include "VkSkeleton.hpp"
#include "buffer.hpp"

namespace Rx {
namespace Component {
    void vkSkeleton_component_on_set(flecs::entity e, VkSkeleton& vkSkeleton) {
        vkSkeleton.buffer = Core::createBufferInterface(
            vkSkeleton.nodeCount,
            sizeof(VkSkeletonNode),
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    void vkSkeleton_component_on_remove(flecs::entity e, VkSkeleton& vkSkeleton) {
        Core::destroyBufferInterface(vkSkeleton.buffer);
    }
}
}
