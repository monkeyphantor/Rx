#include "Skeleton.hpp"

namespace Rx{
    namespace Component{

        void skeleton_on_component_set(flecs::entity entity, Skeleton& skeleton){
            skeleton.vkNodes.resize(skeleton.nodes.size());
            for (size_t i = 0; i < skeleton.nodes.size(); ++i) {
                skeleton.vkNodes[i] = {
                    .isBone_parentIndex_boneIndex = glm::ivec4(skeleton.nodes[i].isBone, skeleton.nodes[i].parentIndex, skeleton.nodes[i].boneIndex, 0),
                    .offset = skeleton.nodes[i].offset
                };
            }
        }
    }
}