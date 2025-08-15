#pragma once
#include "Global.hpp"
#include "flecs.h"
#include <variant>
#include "Skeleton.hpp"
#include "KeyFrameBuffer.hpp"
#include "Node.hpp"
#include "AnimationClip.hpp"
#include "AnimationBone.hpp"
#include "LocalTransform.hpp"
#include "Time.hpp"
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
        flecs::entity skeletonInstance;
        KeyFrameBuffer& keyFrameBuffer;

       
        // void calculateBoneTransforms(const std::vector<KeyFrame>& keyFrames) {
        //     std::vector<glm::mat4> transforms(skeleton.nodes.size(), glm::mat4(1.0f));
        //     std::vector<glm::mat4> nodeTransforms(skeleton.nodes.size(), glm::mat4(1.0f));

        //     for(uint32_t i = 0; i < transforms.size(); i++){
        //         const Node& node = skeleton.nodes[i];
        //         if(node.isBone){
        //             const KeyFrame& keyFrame = keyFrames[node.boneIndex];
        //             transforms[i] = glm::translate(glm::mat4(1.0f), keyFrame.position) * glm::toMat4(keyFrame.rotation) * glm::scale(glm::mat4(1.0f), keyFrame.scaling);
        //         }else{
        //             transforms[i] = node.offset;
        //         }
        //     }

        //     nodeTransforms[0] = transforms[0];
        //     for(uint32_t i = 1; i < transforms.size(); i++){
        //         const int parentIndex = skeleton.nodes[i].parentIndex;
        //         nodeTransforms[i] = nodeTransforms[parentIndex] * transforms[i];
        //     }

        //     for(uint32_t i = 0; i < nodeTransforms.size(); i++){
        //         const Node& node = skeleton.nodes[i];
        //         if(node.isBone){
        //             skeletonBuffer.transforms[i].local = nodeTransforms[i];
        //             skeletonBuffer.transforms[i].bone = nodeTransforms[i] * node.offset;
        //         }else{
        //             skeletonBuffer.transforms[i].local = nodeTransforms[i];
        //             skeletonBuffer.transforms[i].bone = nodeTransforms[i];
        //         }
        //     }
        // }
        
        // void calculateBoneTransforms(const std::vector<AnimationBone>& animationBones, uint32_t& nodeIndex, float animationTime, glm::mat4 parentTransform) {
        //     const Node& node = nodes[nodeIndex];
        //     glm::mat4 nodeTransform;
        //     if(node.isBone){
        //         const AnimationBone& bone = animationBones[node.boneIndex];
        //         auto [position, rotation, scaling] = bone.getLocalTransform(animationTime);
        //         nodeTransform = parentTransform * glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scaling);
        //         skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
        //         skeletonBuffer.transforms[nodeIndex].bone = nodeTransform * node.offset;
        //     }else{
        //         nodeTransform = parentTransform * node.offset;
        //         skeletonBuffer.transforms[nodeIndex].local = nodeTransform;
        //         skeletonBuffer.transforms[nodeIndex].bone = nodeTransform;
        //     }

        //     for(uint32_t i = 0; i < node.numberChildren; i++) {
        //         calculateBoneTransforms(animationBones, ++nodeIndex, animationTime, nodeTransform);
        //     }
        // }

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

        uint32_t getUpperKeyFrameIndex(const std::vector<float>& keyFrameTimes, const float animationTime) {
            uint32_t keyFrameIndex = keyFrameTimes.size() - 1; // Default to the last keyframe
            for (uint32_t i = 0; i < keyFrameTimes.size(); i++) {
                if (animationTime <= keyFrameTimes[i]) {
                    keyFrameIndex = i;
                    break;
                }
            }
            return keyFrameIndex;
        }

        float getScaleFactor(const float lowerKeyFrameTime, const float upperKeyFrameTime, const float animationTime) {
            float scaleFactor = 0.0f;
            float midWayLength = animationTime - lowerKeyFrameTime;
            float framesDiff = upperKeyFrameTime - lowerKeyFrameTime;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        std::vector<VkKeyFrame> interpolateKeyFrames(const std::vector<KeyFrame>& lowerKeyFrames, const std::vector<KeyFrame>& upperKeyFrames, float scaleFactor) {
            std::vector<VkKeyFrame> interpolatedKeyFrames;
            interpolatedKeyFrames.reserve(lowerKeyFrames.size());
            for (size_t i = 0; i < lowerKeyFrames.size(); ++i) {
                VkKeyFrame interpolatedKeyFrame;
                interpolatedKeyFrame.position = glm::vec4(glm::mix(lowerKeyFrames[i].position, upperKeyFrames[i].position, scaleFactor), 1.0f);
                auto quat = glm::slerp(lowerKeyFrames[i].rotation, upperKeyFrames[i].rotation, scaleFactor);
                interpolatedKeyFrame.rotation = glm::vec4(quat.x, quat.y, quat.z, quat.w);
                interpolatedKeyFrame.scaling = glm::vec4(glm::mix(lowerKeyFrames[i].scaling, upperKeyFrames[i].scaling, scaleFactor), 1.0f);
                interpolatedKeyFrames.push_back(interpolatedKeyFrame);
            }
            return interpolatedKeyFrames;
        }

        void operator()(SingleAnimation& state) {
            uint32_t nodeIndex = 0;
            const auto& animationPrefab = skeletonInstance.get<Rx::Component::AnimationClip>(state.animation);
            const auto& keyFrameTimes = animationPrefab.keyFrameTimes;
            uint32_t keyFrameIndex = getUpperKeyFrameIndex(keyFrameTimes, state.animationTime);
            keyFrameIndex = keyFrameIndex * (keyFrameIndex > 0) + 1 * (keyFrameIndex == 0); // Ensure we have a valid index

            const auto& upperKeyFrames = animationPrefab.keyFrames[keyFrameIndex];
            const auto& lowerKeyFrames = animationPrefab.keyFrames[keyFrameIndex - 1];

            float scaleFactor = getScaleFactor(keyFrameTimes[keyFrameIndex - 1], keyFrameTimes[keyFrameIndex], state.animationTime);
            keyFrameBuffer.keyFrames = interpolateKeyFrames(lowerKeyFrames, upperKeyFrames, scaleFactor);
            //calculateBoneTransforms(interpolatedKeyFrames);
            
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

        void update(flecs::entity skeletonInstance, KeyFrameBuffer& keyFrameBuffer){
            std::visit(UpdateVisitor(skeletonInstance, keyFrameBuffer), currentState);
        }
    };

} // namespace Component
} // namespace Rx

