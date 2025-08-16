#pragma once

#include "Global.hpp"

namespace Rx
{
    namespace Core
    {
        VkFence createFence(VkFenceCreateFlags flags);
        void destroyFence(VkFence fence);
    }
}