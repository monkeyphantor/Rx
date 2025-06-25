cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_component STATIC
transform.hpp
transform.cpp
colorMesh.hpp
colorMesh.cpp
colorGraphics.hpp
colorGraphics.cpp
flyingCamera.hpp
velocity.hpp
velocity.cpp
mesh.hpp
mesh.cpp
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