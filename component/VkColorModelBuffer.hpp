#pragma once

#include "global.hpp"
#include "buffer.hpp"
#include "Material.hpp"
#include "flecs.h"

namespace Rx{
    namespace ModelBuffer{
        struct Color
        {
            glm::mat4 transform;
            glm::mat4 normalTransform;
            glm::vec4 albedo;
            glm::vec4 metalRough;       // Base color
            glm::vec4 emissive;
        };
    };

    namespace Component
    {
        struct VkColorModelBuffer
        {
            Core::BufferInterface buffer;

        public:
            void createModelBuffer();
            void destroyModelBuffer();

        public:
        void setModelBuffer(glm::mat4 transform, Material material)
            {
                ModelBuffer::Color bufferCPU;
                bufferCPU.transform = transform;
                bufferCPU.normalTransform = glm::transpose(glm::inverse(transform));
                bufferCPU.albedo = glm::vec4(material.albedo, 1.f);
                bufferCPU.metalRough = glm::vec4(material.metallic, material.roughness, 0.f, 0.f);
                bufferCPU.emissive = glm::vec4(material.emissive, 1.f);

                memcpy(buffer.pMemory, &bufferCPU, sizeof(ModelBuffer::Color));
            }
        };
    }

void model_buffer_component_on_add(flecs::entity e, Rx::Component::VkColorModelBuffer& modelBuffer);
void model_buffer_component_on_remove(flecs::entity e, Rx::Component::VkColorModelBuffer& modelBuffer);

}
