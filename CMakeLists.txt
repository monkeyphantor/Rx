# Rx/CMakeLists.txt

# Set the minimum required version of CMake.
cmake_minimum_required(VERSION 3.15)

# Define the project name, version, and language. This will be the name of the .sln file.
project(RxProject VERSION 1.0 LANGUAGES CXX)

# --- Find External Dependencies ---
# Use find_package to locate the necessary libraries. CMake will fail if they are not found.
# For this to work, you must either install these libraries to a system path
# or tell CMake where to find them using CMAKE_PREFIX_PATH (see instructions below).

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Vulkan SDK. The official SDK sets an environment variable that CMake uses.
find_package(Vulkan REQUIRED)

# Find the GLFW3 library.
find_package(glfw3 REQUIRED)

# Find the GLM mathematics library.
find_package(glm REQUIRED)

# --- Build Internal Libraries ---
# Process the subdirectories to create our 'component_lib' and 'core_lib' targets.
add_subdirectory(flecs)
add_subdirectory(core)
add_subdirectory(component)
add_subdirectory(application)
# --- Build The Main Executable ---
# Define the main executable target, "RxWorld".
add_executable(RxWorld main.cpp)


# --- Link All Libraries ---
# Link RxWorld against all the libraries it needs. Modern CMake uses imported targets
# (like Vulkan::Vulkan, glm::glm, glfw) which also handle include directories automatically.
target_link_libraries(RxWorld PRIVATE
    # Our internal libraries
    rx_core
    rx_component
    rx_flecs
    rx_app
    # External dependencies
    Vulkan::Vulkan  # The imported target for Vulkan
    glm::glm        # The imported target for GLM
    glfw            # The imported target for GLFW
)

# Optional: Organize projects in Visual Studio into folders for clarity.
set_property(TARGET RxWorld PROPERTY FOLDER "Executables")