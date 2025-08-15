#pragma once

#include "Global.hpp"
#include "Command.hpp"

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