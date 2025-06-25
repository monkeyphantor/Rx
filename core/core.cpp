#include "core.hpp"
#include "mutex.hpp"

namespace Rx
{
namespace Core
{
    void createCore()
    {
        createWindow();
        Time::createTime();
        Input::createInput();

        createInstance();
        createSurface();
        pickPhysicalDevice();
        createDevice();
        getQueue();
        createAllocator();
        createDescriptorPool();


        Core::command[0] = createCommand();
        Core::command[1] =  createCommand();

        Core::single_command = createSingleCommand();

        createEye();

        createSwapchain();
        createRenderPass();
        createFramebuffers();
    }

    void destroyCore()
    {
        destroyFramebuffers();
        destroyRenderPass();
        destroySwapchain();

        destroyEye();

        destroySingleCommand(Core::single_command);   
        destroyCommand(Core::command[1]);
        destroyCommand(Core::command[0]);

        
        destroyDescriptorPool();
        destroyAllocator();
        destroyDevice();
        destroySurface();
        destroyInstance();

        destroyWindow();
    }

    bool updateCore()
    {
        Rx::Core::commandIndex = (Rx::Core::commandIndex + 1) % 2;
        Rx::Core::waitForCommand(Rx::Core::command[Rx::Core::commandIndex]);

        glfwPollEvents();
        
        Time::updateTime();
        Input::updateInput();

        return !glfwWindowShouldClose(pGLFWwindow);
    }

}
}