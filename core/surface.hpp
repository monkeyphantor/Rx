#pragma once

#include "Global.hpp"

namespace Rx
{
    namespace Core
    {
        inline VkSurfaceKHR vkSurfaceKHR;

        void createSurface();
        void destroySurface();
    }
}