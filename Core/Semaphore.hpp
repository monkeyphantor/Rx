#pragma once

#include "Global.hpp"

namespace Rx
{
    namespace Core
    {
        VkSemaphore createSemaphore();
        void destroySemaphore(VkSemaphore semaphore);
    }
}