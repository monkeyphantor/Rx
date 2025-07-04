cmake_minimum_required(VERSION 3.16.3)

project(rx)

add_library(rx_app STATIC
application.hpp
application.cpp
level.hpp
gameWorldBase.hpp
gameWorldBase.cpp
loadingScreen.hpp
gameWorld.hpp
gameWorld.cpp
RxDefaultLoadingScreen.hpp
RxDefaultGameWorld.hpp
)

target_include_directories(rx_app PUBLIC ../core ../flecs ../component)
target_link_libraries(rx_app PUBLIC
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
    rx_core
    rx_flecs
    rx_component
)