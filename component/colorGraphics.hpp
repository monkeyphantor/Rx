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
            VkDescriptorSetLayout vkDescriptorSetLayout; 
            VkDescriptorSet vkDescriptorSet;
            VkPipelineLayout vkPipelineLayout;
            VkPipeline vkPipeline;

        public:
            void createModelBuffer();
            void destroyModelBuffer();
            void createDescriptorSet();
            void destroyDescriptorSet();
            void createGraphicsPipeline
            (Core::Shader& vertexShader, 
            Core::Shader& fragmentShader);
            void destroyGraphicsPipeline();

        public:
             const VkDescriptorSet& getDescriptorSet()
            {
                return vkDescriptorSet;
            }

            const VkPipelineLayout& getPipelineLayout()
            {
                return vkPipelineLayout;
            }

            const VkPipeline& getGraphicsPipeline()
            {
                return vkPipeline;
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


inline Rx::Core::Shader vertexShader("./shader/testShader3.vert.spv");
inline Rx::Core::Shader fragmentShader("./shader/testShader3.frag.spv");

void graphics_component_on_add(flecs::entity e, Rx::Component::ColorGraphics& graphics);

void graphics_component_on_remove(flecs::entity e, Rx::Component::ColorGraphics& graphics);


