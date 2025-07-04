cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_component STATIC
transform.hpp
transform.cpp
ColorMesh.hpp
ColorMesh.cpp
VkColorMesh.hpp
VkColorMesh.cpp
VkColorModelBuffer.hpp
VkColorModelBuffer.cpp
VkColorModelDescriptorSet.hpp
VkColorModelDescriptorSet.cpp
flyingCamera.hpp
velocity.hpp
velocity.cpp
meshArray.hpp
meshArray.cpp
colorArrayGraphics.hpp
colorArrayGraphics.cpp
colorMeshArray.hpp
colorMeshArray.cpp
VkInstancedColorModelBuffer.hpp
VkInstancedColorModelBuffer.cpp
indirectBuffer.hpp
indirectBuffer.cpp
VkInstancedColorModelDescriptorSet.hpp
VkInstancedColorModelDescriptorSet.cpp
)

target_include_directories(rx_component PUBLIC ../core ../flecs)
target_link_libraries(rx_component PUBLIC
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
    rx_flecs
    rx_core
)