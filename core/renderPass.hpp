#pragma once

#include "global.hpp"
#include "command.hpp"

namespace Rx
{
    namespace Core
    {
        inline VkRenderPass vkRenderPass;

        void createRenderPass();
        void destroyRenderPass();
        void beginRenderPass(Command command);
        void endRenderPass(Command command);
    }
}