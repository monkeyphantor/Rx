#pragma once

#include "global.hpp"
#include "command.hpp"

namespace Rx
{
    namespace Core
    {
        inline VkQueue vkQueue;

        void getQueue();
        void submitSingleCommand(SingleCommand command);
        void submitGraphicsCommand(Command command);
        void presentGraphics(Command command);
    }
}