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
VkIndirectBuffer.hpp
VkIndirectBuffer.cpp
VkInstancedColorModelDescriptorSet.hpp
VkInstancedColorModelDescriptorSet.cpp
TextureArray.hpp
TextureArray.cpp
VkTextureArray.hpp
VkTextureArray.cpp
TextureMeshArray.hpp
TextureMeshArray.cpp
VkTextureMeshArray.hpp
VkTextureMeshArray.cpp
VkTransformBuffer.hpp
VkTransformBuffer.cpp
VkTextureMaterialBuffer.hpp
VkTextureMaterialBuffer.cpp
VkTextureModelDescriptorSet.hpp
VkTextureModelDescriptorSet.cpp
AnimationBone.hpp
AnimationBone.cpp
AnimationClip.hpp
AnimationClip.cpp
AnimationStateMachine.hpp
AnimationStateMachine.cpp
SkeletonMeshArray.hpp
SkeletonMeshArray.cpp
VkSkeletonMeshArray.hpp
VkSkeletonMeshArray.cpp
VkSkeletonArrayBuffer.hpp
VkSkeletonArrayBuffer.cpp
VkSkeletonModelDescriptorSet.hpp
VkSkeletonModelDescriptorSet.cpp

    VkKeyFrameArrayBuffer.hpp
    VkKeyFrameArrayBuffer.cpp
    VkSkeleton.hpp
    VkSkeleton.cpp
    VkSkeletonModelCompDescriptorSet.hpp
    VkSkeletonModelCompDescriptorSet.cpp
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