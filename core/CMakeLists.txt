cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_core STATIC
window.hpp
window.cpp
time.hpp
time.cpp
input.hpp
input.cpp
instance.hpp
instance.cpp
surface.hpp
surface.cpp
physicalDevice.hpp
physicalDevice.cpp
device.hpp
device.cpp
queue.hpp
queue.cpp
command.hpp
command.cpp
allocator.hpp
allocator.cpp
descriptorPool.hpp
descriptorPool.cpp
semaphore.hpp
semaphore.cpp
fence.hpp
fence.cpp
image.hpp
image.cpp
buffer.hpp
buffer.cpp
eye.hpp
eye.cpp
swapchain.hpp
swapchain.cpp
renderPass.hpp
renderPass.cpp
framebuffers.hpp
framebuffers.cpp
shader.hpp
shader.cpp
core.hpp
core.cpp)


target_link_libraries(rx_core PUBLIC
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
)