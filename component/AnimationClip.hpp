#pragma once
#include "global.hpp"
#include "AnimationBone.hpp"

namespace Rx {
namespace Component {

    struct KeyFrame{
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scaling;
    };

    struct AnimationClip {
        float duration;
        float ticksPerSecond;
        float durationInSeconds;
        std::vector<std::vector<KeyFrame>> keyFrames;
        std::vector<float> keyFrameTimes;
    };

}
}