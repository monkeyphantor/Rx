#pragma once

#include "global.hpp"
#include "NodeTransform.hpp"
#include "VkSkeletonArrayBuffer.hpp"

namespace Rx {
    namespace Component {
        struct SkeletonBuffer {
            std::vector<NodeTransform> transforms;

            void toVkSkeletonArrayBuffer(VkSkeletonArrayBuffer& arrayBuffer, uint32_t instanceNumber) const {
                NodeTransform* pTransforms = static_cast<NodeTransform*>(arrayBuffer.buffer.pMemory);
                std::memcpy(pTransforms + instanceNumber * 256, transforms.data(), transforms.size() * sizeof(NodeTransform));
            }
        };

    }
}