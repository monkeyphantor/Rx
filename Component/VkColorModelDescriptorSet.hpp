#pragma once

#include "../Core/Global.hpp"
#include "flecs.h"
#include "VkColorModelBuffer.hpp"

namespace Rx
{
    namespace Component
    {
        struct VkColorModelDescriptorSet
        {
        
        protected:
            VkDescriptorSet vkDescriptorSet;
           
        public:
            void createDescriptorSet(Rx::Component::VkColorModelBuffer& buffer);
            void destroyDescriptorSet();
            
        public:
             const VkDescriptorSet& getDescriptorSet()
            {
                return vkDescriptorSet;
            }            
        };
    }
}




void graphics_component_on_add(flecs::entity e,Rx::Component::VkColorModelBuffer& modelBuffer, Rx::Component::VkColorModelDescriptorSet& graphics);

void graphics_component_on_remove(flecs::entity e, Rx::Component::VkColorModelDescriptorSet& graphics);


