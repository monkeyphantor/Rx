#pragma once

#include "global.hpp"

namespace Rx
{
    namespace Core
    {
        VkSemaphore createSemaphore();
        void destroySemaphore(VkSemaphore semaphore);
    }
}