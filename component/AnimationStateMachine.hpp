#pragma once
#include "global.hpp"
#include "flecs.h"
#include <variant>
#include "Skeleton.hpp"
#include "SkeletonBuffer.hpp"
#include "Node.hpp"
#include "AnimationClip.hpp"
#include "AnimationBone.hpp"
#include "LocalTransform.hpp"
#include "time.hpp"
#include "Transform.hpp"
#include "NodeChildren.hpp"
#include "SkeletonInstance.hpp"
namespace Rx {
namespace Component {

    struct SingleAnimation{
        flecs::entity animation;
        float animationTime;
        float animationSpeed;
        float duration;
        float ticksPerSecond;
    };

    struct BlendSpace1D{
        flecs::entity animation1;
        flecs::entity animation2;
        float blendFactor = 0.0f;
        float animationTime1 = 0.0f;
        float animationTime2 = 0.0f;
    };

    struct UpdateVisitor{
        SkeletonBuffer& skeletonBuffer;
        const std::vector<Node>& nodes;
        flecs::entity animationEntity;

        void calculateBoneTransforms(const std::vector<AnimationBone>& animationBones, uint32_t& nodeIndex, float animationTime, glm::mat4 parentTransform) {
            const Node& node = nodes[nodeIndex];
            glm::mat4 nodeTransform;
            if(node.isBone){
                const AnimationBone& bone = animationBones[node.boneIndex];
                auto [position, rotation, scaling] = bone.getLocalTransform(animationTime);
                nodeTransform = parentTransform * glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scaling);
                skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
                skeletonBuffer.transforms[nodeIndex].bone = nodeTransform * node.offset;
            }else{
                nodeTransform = parentTransform * node.offset;
                skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
                skeletonBuffer.transforms[nodeIndex].bone = nodeTransform;
            }

            for(uint32_t i = 0; i < node.numberChildren; i++) {
                calculateBoneTransforms(animationBones, ++nodeIndex, animationTime, nodeTransform);
            }
        }

        // void calculateBoneTransforms(const SingleAnimation& animation, uint32_t& nodeIndex, glm::mat4 parentTransform) {
        //     glm::mat4 nodeTransform;
        //     flecs::entity nodeEntity = skeletonInstance.nodeEntities[nodeIndex];
        //     if(nodeEntity.has<Rx::Component::BoneIndex>()) {
        //         auto& bone = nodeEntity.get_mut<Rx::Component::AnimationBone>(animation.animation);
                
        //         auto [position, rotation, scaling] = bone.getLocalTransform(animation.animationTime);
        //         nodeTransform = parentTransform * glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scaling);
        //         skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
        //         skeletonBuffer.transforms[nodeIndex].bone = nodeTransform * nodeEntity.get<Rx::Component::InvBindPose>().invBindPose;
        //         nodeEntity.set<Rx::Component::Transform>(modelTransform * Transform::fromGlmMat4(nodeTransform));
        //     } else {
        //         nodeTransform = parentTransform * nodeEntity.get<Rx::Component::LocalTransform>().getTransformMatrix();
        //         skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
        //         skeletonBuffer.transforms[nodeIndex].bone = nodeTransform;
        //         nodeEntity.set<Rx::Component::Transform>(modelTransform * Transform::fromGlmMat4(nodeTransform));
        //     }
        //     const NodeChildren& nodeChildren = nodeEntity.get<Rx::Component::NodeChildren>();
        //     for(uint32_t i = 0; i < nodeChildren.numberChildren; i++) {
        //         calculateBoneTransforms(animation, ++nodeIndex, nodeTransform);
        //     }

        //     // nodeEntity.children(world.lookup("IsChildNodeOf"), [&](flecs::entity child) {
        //     //     calculateBoneTransforms(animation, child, nodeTransform);
        //     // });
        // }

        void operator()(SingleAnimation& state) {
            uint32_t nodeIndex = 0;
            auto& animationBones = animationEntity.get<Rx::Component::AnimationClip>(state.animation).bones;
            calculateBoneTransforms(animationBones, nodeIndex, state.animationTime, glm::mat4(1.f));
            state.animationTime += state.ticksPerSecond * state.animationSpeed * Time::deltaTime;
            state.animationTime = fmod(state.animationTime, state.duration);
        }

        void operator()(BlendSpace1D& state) {
            // Update logic for BlendSpace1D
            RX_LOGE("AnimationStateMachine", "BlendSpace1D not implemented yet", "UpdateVisitor");
        }
    };
    
    struct AnimationStateMachine {
        std::variant<
            SingleAnimation,
            BlendSpace1D
        > currentState;

        std::map<std::string, 
        std::variant<
            SingleAnimation,
            BlendSpace1D
        >> animationStates;

        void addAnimationState(const std::string& name, const std::variant<SingleAnimation, BlendSpace1D>& state) {
            animationStates[name] = state;
        }

        void setCurrentState(const std::string& name) {
            auto it = animationStates.find(name);
            if (it != animationStates.end()) {
                currentState = it->second;
            } else {
                RX_LOGE("AnimationStateMachine", "setCurrentState", 
                        ("Animation state '" + name + "' not found").c_str());
            }
        }

        void update(flecs::world& world, SkeletonBuffer& skeletonBuffer, const std::vector<Node>& nodes, flecs::entity instance){
            flecs::entity animationEntity;
            instance.children(world.lookup("IsAnimationOf"), [&](flecs::entity child) {
                animationEntity = child;
            });

            std::visit(UpdateVisitor(skeletonBuffer, nodes, animationEntity), currentState);
        }
    };

} // namespace Component
} // namespace Rx

