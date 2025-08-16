#pragma once

#include "Global.hpp"
#include "Command.hpp"

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