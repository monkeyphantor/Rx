#pragma once

#include "VkKeyFrameArrayBuffer.hpp"
#include "AnimationClip.hpp"

namespace Rx{
    namespace Component {
        struct KeyFrameBuffer {
            std::vector<VkKeyFrame> keyFrames;
        };
    }
}