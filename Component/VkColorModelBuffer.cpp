#include "VkColorModelBuffer.hpp"

namespace Rx{
     void model_buffer_component_on_add(flecs::entity e, Rx::Component::VkColorModelBuffer& modelBuffer)
        {
            modelBuffer.createModelBuffer();
        }
        
        void model_buffer_component_on_remove(flecs::entity e, Rx::Component::VkColorModelBuffer& modelBuffer)
        {
            modelBuffer.destroyModelBuffer();
        }
       
        namespace Component{

        void VkColorModelBuffer::createModelBuffer()
        {
            buffer = 
            Core::createBufferInterface
            (1, sizeof(ModelBuffer::Color),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        }

        void VkColorModelBuffer::destroyModelBuffer()
        {
            Core::destroyBufferInterface(buffer);
        }
    }
}