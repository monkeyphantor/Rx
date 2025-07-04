#pragma once

#include "../core/buffer.hpp"
#include "../flecs/include/flecs.h"
namespace Rx {
    namespace Component {
        struct VkInstancedColorModelBuffer {
            uint32_t maxNumberInstances{0};
            uint32_t numberInstances{0}; // Current number of instances in the buffer
            Core::BufferInterface instanceBuffer; // Buffer for instance data (e.g., transformation matrices)
        };

    void colorMeshInstanceBuffer_on_set(flecs::entity e, Rx::Component::VkInstancedColorModelBuffer& instanceBuffer);   
    void colorMeshInstanceBuffer_on_remove(flecs::entity e, Rx::Component::VkInstancedColorModelBuffer& instanceBuffer);
} // namespace Component
} // namespace Rx