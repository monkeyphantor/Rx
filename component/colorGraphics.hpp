#pragma once

#include "../core/global.hpp"
#include "../core/buffer.hpp"
#include "../core/shader.hpp"
#include "colorGraphics.hpp"
#include "../core/physicalDevice.hpp"
#include "../core/device.hpp"
#include "../core/descriptorPool.hpp"
#include "../core/renderPass.hpp"
#include "../core/eye.hpp"
#include "colorMesh.hpp"
#include "../core/mutex.hpp"

namespace Rx
{
    namespace ModelBuffer
    {
        struct Color
        {
            glm::mat4 transform;
        };
    };

    namespace Component
    {
        struct ColorGraphics
        {
        
        
        protected:
            bool destroyed{false};

        protected:
            Core::BufferInterface modelBuffer;
            VkDescriptorSet vkDescriptorSet;
           
        public:
            void createModelBuffer();
            void destroyModelBuffer();
            void createDescriptorSet();
            void destroyDescriptorSet();
            
        public:
             const VkDescriptorSet& getDescriptorSet()
            {
                return vkDescriptorSet;
            }

           
            void setModelBuffer(glm::mat4 transform)
            {
                ModelBuffer::Color buffer;
                buffer.transform = transform;

                memcpy(modelBuffer.pMemory, &buffer, sizeof(ModelBuffer::Color));
            }
        };
    }
}




void graphics_component_on_add(flecs::entity e, Rx::Component::ColorGraphics& graphics);

void graphics_component_on_remove(flecs::entity e, Rx::Component::ColorGraphics& graphics);


