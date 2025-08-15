#include "VkSkeleton.hpp"

namespace Rx{
    namespace Component{
        void vkSkeleton_component_on_add(flecs::entity e, Skeleton& skeleton, VkSkeleton& vkSkeleton){

            vkSkeleton.skeleton = Core::createBufferInterface(
                256,
                sizeof(VkNode),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );

            memcpy(vkSkeleton.skeleton.pMemory, skeleton.vkNodes.data(), skeleton.vkNodes.size() * sizeof(VkNode));
        }
        void vkSkeleton_component_on_remove(flecs::entity e, VkSkeleton& vkSkeleton){
            Core::destroyBufferInterface(vkSkeleton.skeleton);
        }

    }
}