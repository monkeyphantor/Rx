#pragma once

#include "Global.hpp"
#include "AnimationClip.hpp"
namespace Rx{
    namespace Component{
        struct AnimationMap {
            std::map<std::string, std::shared_ptr<AnimationClip>> animations;
            std::map<std::string, uint32_t> boneIndices;
        };
    }
}