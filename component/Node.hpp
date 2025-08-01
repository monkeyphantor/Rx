#pragma once
#include "global.hpp"

namespace Rx {
    namespace Component {
        struct Node {
            bool isBone;
            char name[128];
            glm::mat4 offset;
            int numberChildren;
            int nodeIndex;
            int boneIndex;
        };

        struct NodeIndex {
            uint32_t index;
        };
        
        struct BoneIndex {
            uint32_t index;
        };
    
        struct InvBindPose {
            glm::mat4 invBindPose;
        };
    }
}