cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_asset STATIC
LoadTexture.hpp
LoadTexture.cpp
LoadTextureModel.hpp
LoadTextureModel.cpp
)

target_include_directories(rx_asset PUBLIC ../core ../flecs ../component ../application)
target_link_libraries(rx_asset PUBLIC
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
    Jolt::Jolt
    rx_core
    rx_flecs
    rx_component
    rx_app
    
)