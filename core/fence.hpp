#pragma once

#include "global.hpp"

namespace Rx
{
    namespace Core
    {
        VkFence createFence(VkFenceCreateFlags flags);
        void destroyFence(VkFence fence);
    }
}