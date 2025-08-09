#pragma once
#include "global.hpp"

namespace Rx {
    namespace Component {
        struct Node {
            bool isBone;
            glm::mat4 offset;
            int numberChildren;
            int nodeIndex;
            int boneIndex;
            int parentIndex;
        };

        struct NodeName{
            char name[128];
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