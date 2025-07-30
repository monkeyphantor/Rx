#pragma once
#include "global.hpp"
#include "AnimationBone.hpp"

namespace Rx {
namespace Component {

    struct AnimationClip {
        float duration;
        float ticksPerSecond;
        float durationInSeconds;
    };

}
}