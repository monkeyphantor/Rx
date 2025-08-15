cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_app STATIC
Application.hpp
Application.cpp
Level.hpp
GameWorldBase.hpp
GameWorldBase.cpp
LoadingScreen.hpp
GameWorld.hpp
GameWorld.cpp
PhysicsGameWorld.hpp
PhysicsGameWorld.cpp
RxDefaultLoadingScreen.hpp
RxDefaultGameWorld.hpp
)

target_include_directories(rx_app PUBLIC ../Core ../Flecs ../Component)
target_link_libraries(rx_app PUBLIC
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
    Jolt::Jolt
    rx_core
    rx_flecs
    rx_component
)
