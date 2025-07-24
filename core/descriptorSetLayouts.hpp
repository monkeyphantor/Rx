#pragma once

#include "../core/global.hpp"

namespace Rx
{
    namespace Core
    {

        
        void createDescriptorSetLayouts();
        void destroyDescriptorSetLayouts();



        inline VkDescriptorSetLayout colorMeshDescriptorSetLayout;
        void createColorMeshDescriptorSetLayout();
        void destroyColorMeshDescriptorSetLayout();
        
        inline VkDescriptorSetLayout colorMeshArrayDescriptorSetLayout;
        void createColorMeshArrayDescriptorSetLayout();
        void destroyColorMeshArrayDescriptorSetLayout();

        inline VkDescriptorSetLayout instancedColorMeshDescriptorSetLayout;
        void createInstancedColorMeshDescriptorSetLayout();
        void destroyInstancedColorMeshDescriptorSetLayout();

        inline VkDescriptorSetLayout textureModelDescriptorSetLayout;
        void createTextureModelDescriptorSetLayout();
        void destroyTextureModelDescriptorSetLayout();
        
    }
}
