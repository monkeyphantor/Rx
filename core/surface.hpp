#pragma once

#include "global.hpp"

namespace Rx
{
    namespace Core
    {
        inline VkSurfaceKHR vkSurfaceKHR;

        void createSurface();
        void destroySurface();
    }
}