#include "Surface.hpp"
#include "Window.hpp"
#include "Instance.hpp"

namespace Rx
{
namespace Core
{
    void createSurface()
    {
        RX_CHECK_VULKAN
        (glfwCreateWindowSurface
        (vkInstance,
        pGLFWwindow,
        nullptr,
        &vkSurfaceKHR),
        "createSurface",
        "glfwCreateWindowSurface")

        #ifdef RX_DEBUG
        RX_LOGI("VkSurfaceKHR" , "created", "")
        #endif
    }

    void destroySurface()
    {
        vkDestroySurfaceKHR
        (vkInstance,
        vkSurfaceKHR,
        nullptr);
    }
}
}