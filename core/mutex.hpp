#pragma once
#include <mutex>

namespace Rx{
    namespace Core{
    inline std::mutex vulkanCommandMutex; // Global mutex for thread safety
}
}

#define RX_VK_MUTEX(Expression)\
{\
    Rx::Core::vulkanCommandMutex.lock();\
    Expression;\
    Rx::Core::vulkanCommandMutex.unlock();\
}