#pragma once

#include "../core/buffer.hpp"
#include "../flecs/include/flecs.h"
namespace Rx {
    namespace Component {
        struct ColorMeshInstanceBuffer {
            uint32_t maxNumberInstances{0};
            Core::BufferInterface instanceBuffer; // Buffer for instance data (e.g., transformation matrices)
        };

    void colorMeshInstanceBuffer_on_set(flecs::entity e, Rx::Component::ColorMeshInstanceBuffer& instanceBuffer);   
    void colorMeshInstanceBuffer_on_remove(flecs::entity e, Rx::Component::ColorMeshInstanceBuffer& instanceBuffer);
} // namespace Component
} // namespace Rx