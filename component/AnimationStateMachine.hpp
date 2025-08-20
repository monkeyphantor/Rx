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
#include <map>
#include <iostream> // For demonstration output

// Forward-declare AnimationStateMachine to resolve circular dependency in UpdateVisitor
namespace Rx {
namespace Component {
    struct AnimationStateMachine;
}
}

namespace Rx {
namespace Component {

    // --- Animation State Structs (Unchanged) ---
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
        float ticksPerSecond1 = 1.0f;
        float animationSpeed1 = 1.0f;
        float duration1 = 1.0f;
        
        float animationTime2 = 0.0f;
        float ticksPerSecond2 = 1.0f;
        float animationSpeed2 = 1.0f;
        float duration2 = 1.0f;
    };

    struct Transition{
        std::string fromState;
        std::string toState;

        float time = 0.0f;
        float ticksPerSecond = 1.0f;
        float speed = 1.0f;
        float duration = 1.0f;
    };

    // --- Update Visitor and Base (Unchanged from previous correct version) ---
    struct UpdateVisitorBase{
        flecs::entity skeletonInstance;
        
        uint32_t getUpperKeyFrameIndex(const std::vector<float>& keyFrameTimes, const float animationTime);

        float getScaleFactor(const float lowerKeyFrameTime, const float upperKeyFrameTime, const float animationTime);
        
        std::vector<KeyFrame> interpolateKeyFrames(const std::vector<KeyFrame>& lowerKeyFrames, const std::vector<KeyFrame>& upperKeyFrames, float scaleFactor);

        
        std::vector<KeyFrame> getAndUpdatePose(flecs::entity animation, float& animationTime, float ticksPerSecond, float animationSpeed, float duration);
    };

    struct UpdateVisitor : public UpdateVisitorBase {
        AnimationStateMachine& stateMachine;

        std::vector<KeyFrame> operator()(SingleAnimation& state);
        std::vector<KeyFrame> operator()(BlendSpace1D& state);
        std::vector<KeyFrame> operator()(Transition& state);

    private:
        std::vector<KeyFrame> updateAndGetPose(std::variant<SingleAnimation, BlendSpace1D, Transition>& state);

        std::vector<KeyFrame> handleState(SingleAnimation& state);

        std::vector<KeyFrame> handleState(BlendSpace1D& state);

        std::vector<KeyFrame> handleState(Transition& state);
    };

    // --- MODIFIED AnimationStateMachine ---
    struct AnimationStateMachine {
        std::string currentStateName; // NEW: Track the name of the current state
        std::variant<
            SingleAnimation,
            BlendSpace1D,
            Transition
        > currentState;

        std::map<std::string, 
        std::variant<
            SingleAnimation,
            BlendSpace1D,
            Transition
        >> animationStates;

        void addAnimationState(const std::string& name, const std::variant<SingleAnimation, BlendSpace1D, Transition>& state);

        void setCurrentState(const std::string& name);
        
        // NEW: Smartly triggers a transition from the current state.
        void triggerTransition(const std::string& transitionName);

        void update(flecs::entity skeletonInstance, KeyFrameBuffer& keyFrameBuffer);

        void convertToVkKeyFrames(const std::vector<KeyFrame>& keyFrames, std::vector<VkKeyFrame>& vkKeyFrames);
    };

    inline uint32_t UpdateVisitorBase::getUpperKeyFrameIndex(const std::vector<float>& keyFrameTimes, const float animationTime) {
            uint32_t keyFrameIndex = keyFrameTimes.size() - 1;
            for (uint32_t i = 0; i < keyFrameTimes.size(); i++) {
                if (animationTime <= keyFrameTimes[i]) {
                    keyFrameIndex = i;
                    break;
                }
            }
            return keyFrameIndex;
        }

        inline float UpdateVisitorBase::getScaleFactor(const float lowerKeyFrameTime, const float upperKeyFrameTime, const float animationTime) {
            float scaleFactor = 0.0f;
            float midWayLength = animationTime - lowerKeyFrameTime;
            float framesDiff = upperKeyFrameTime - lowerKeyFrameTime;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }
        
        inline std::vector<KeyFrame> UpdateVisitorBase::interpolateKeyFrames(const std::vector<KeyFrame>& lowerKeyFrames, const std::vector<KeyFrame>& upperKeyFrames, float scaleFactor) {
            std::vector<KeyFrame> interpolatedKeyFrames;
            interpolatedKeyFrames.reserve(lowerKeyFrames.size());
            for (size_t i = 0; i < lowerKeyFrames.size(); ++i) {
                KeyFrame interpolatedKeyFrame;
                interpolatedKeyFrame.position = glm::mix(lowerKeyFrames[i].position, upperKeyFrames[i].position, scaleFactor);
                interpolatedKeyFrame.rotation = glm::slerp(glm::quat(lowerKeyFrames[i].rotation), glm::quat(upperKeyFrames[i].rotation), scaleFactor);
                interpolatedKeyFrame.scaling = glm::mix(lowerKeyFrames[i].scaling, upperKeyFrames[i].scaling, scaleFactor);
                interpolatedKeyFrames.push_back(interpolatedKeyFrame);
            }
            return interpolatedKeyFrames;
        }

        
        inline std::vector<KeyFrame> UpdateVisitorBase::getAndUpdatePose(flecs::entity animation, float& animationTime, float ticksPerSecond, float animationSpeed, float duration){
            const auto& animationPrefab = skeletonInstance.get<Rx::Component::AnimationClip>(animation);
            const auto& keyFrameTimes = animationPrefab.keyFrameTimes;
            
            uint32_t keyFrameIndex = getUpperKeyFrameIndex(keyFrameTimes, animationTime);
            keyFrameIndex = keyFrameIndex * (keyFrameIndex > 0) + 1 * (keyFrameIndex == 0);

            const auto& upperKeyFrames = animationPrefab.keyFrames[keyFrameIndex];
            const auto& lowerKeyFrames = animationPrefab.keyFrames[keyFrameIndex - 1];

            float scaleFactor = getScaleFactor(keyFrameTimes[keyFrameIndex - 1], keyFrameTimes[keyFrameIndex], animationTime);
            std::vector<KeyFrame> keyFrames = interpolateKeyFrames(lowerKeyFrames, upperKeyFrames, scaleFactor);
            
            animationTime += ticksPerSecond * animationSpeed * Time::deltaTime;
            animationTime = fmod(animationTime, duration);

            return keyFrames;
        }

        inline std::vector<KeyFrame> UpdateVisitor::operator()(SingleAnimation& state) {
            return handleState(state);
        }

        inline std::vector<KeyFrame> UpdateVisitor::operator()(BlendSpace1D& state) {
            return handleState(state);
        }
        inline std::vector<KeyFrame> UpdateVisitor::operator()(Transition& state) {
            std::vector<KeyFrame> finalPose = handleState(state);

            if (state.time / state.duration >= 1.0f) {
                // When transition finishes, set current state to the transition's destination
                stateMachine.setCurrentState(state.toState);
            }

            return finalPose;
        }
        
        inline std::vector<KeyFrame> UpdateVisitor::updateAndGetPose(std::variant<SingleAnimation, BlendSpace1D, Transition>& state) {
            return std::visit([this](auto& specificState) -> std::vector<KeyFrame> {
                return this->handleState(specificState);
            }, state);
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(SingleAnimation& state) {
            return getAndUpdatePose(state.animation, state.animationTime, state.ticksPerSecond, state.animationSpeed, state.duration);
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(BlendSpace1D& state) {
            std::vector<KeyFrame> keyFrames1 = getAndUpdatePose(state.animation1, state.animationTime1, state.ticksPerSecond1, state.animationSpeed1, state.duration1);
            std::vector<KeyFrame> keyFrames2 = getAndUpdatePose(state.animation2, state.animationTime2, state.ticksPerSecond2, state.animationSpeed2, state.duration2);
            return interpolateKeyFrames(keyFrames1, keyFrames2, state.blendFactor);
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(Transition& state) {
            auto& fromStateVariant = stateMachine.animationStates.at(state.fromState);
            auto& toStateVariant = stateMachine.animationStates.at(state.toState);

            std::vector<KeyFrame> keyFramesFrom = updateAndGetPose(fromStateVariant);
            std::vector<KeyFrame> keyFramesTo = updateAndGetPose(toStateVariant);
            
            float blendFactor = std::min(1.0f, state.time / state.duration);
            std::vector<KeyFrame> finalKeyFrames = interpolateKeyFrames(keyFramesFrom, keyFramesTo, blendFactor);

            state.time += state.ticksPerSecond * state.speed * Time::deltaTime;
            return finalKeyFrames;
        }

        inline void AnimationStateMachine::addAnimationState(const std::string& name, const std::variant<SingleAnimation, BlendSpace1D, Transition>& state) {
            animationStates[name] = state;
        }

        inline void AnimationStateMachine::setCurrentState(const std::string& name) {
            auto it = animationStates.find(name);
            if (it != animationStates.end()) {
                currentState = it->second;
                currentStateName = name; // Keep the name in sync
                 if (std::holds_alternative<Transition>(currentState)) {
                    // Reset transition time when setting it directly
                    std::get<Transition>(currentState).time = 0.0f;
                }
            } else {
                RX_LOGE("AnimationStateMachine", "setCurrentState", 
                        ("Animation state '" + name + "' not found").c_str());
            }
        }

        inline void AnimationStateMachine::update(flecs::entity skeletonInstance, KeyFrameBuffer& keyFrameBuffer){
            UpdateVisitor visitor{skeletonInstance, *this};
            std::vector<KeyFrame> keyFrames = std::visit(visitor, currentState);
            convertToVkKeyFrames(keyFrames, keyFrameBuffer.keyFrames);
        }

        inline void AnimationStateMachine::convertToVkKeyFrames(const std::vector<KeyFrame>& keyFrames, std::vector<VkKeyFrame>& vkKeyFrames) {
            for (uint32_t i = 0; i < keyFrames.size(); ++i) {
                const KeyFrame& keyFrame = keyFrames[i];
                VkKeyFrame vkKeyFrame;
                vkKeyFrame.position = glm::vec4(keyFrame.position, 1.0f);
                vkKeyFrame.rotation = glm::vec4(keyFrame.rotation.x, keyFrame.rotation.y, keyFrame.rotation.z, keyFrame.rotation.w);
                vkKeyFrame.scaling = glm::vec4(keyFrame.scaling, 1.0f);
                vkKeyFrames[i] = vkKeyFrame;
            }
        }

}
}