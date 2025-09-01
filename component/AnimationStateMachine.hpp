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


    struct SingleAnimation{
        AnimationClip* animation;
        float animationTime;
        float animationSpeed;
        float duration;
        float ticksPerSecond;
    };

    struct BlendSpace1D{
        AnimationClip* animations[5];
        uint32_t numberAnimationPoints = 0;
        float animationPoints[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        int8_t nearestPointsIndices[2];
        float animationPointState = 0.0f;
        float blendFactor = 0.0f;

        float animationTime = 0.f;
        float ticksPerSecond[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
        float animationSpeed[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
        float duration[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};


        void calculateNearestAnimationPoints(){
            // Find the two animationPoints closest to blendFactor
            float minDist1 = std::numeric_limits<float>::max();
            float minDist2 = std::numeric_limits<float>::max();
            int idx1 = -1, idx2 = -1;

            for (int i = 0; i < numberAnimationPoints; ++i) {
                float dist = std::abs(animationPoints[i] - animationPointState);
                if (dist < minDist1) {
                    minDist2 = minDist1;
                    idx2 = idx1;
                    minDist1 = dist;
                    idx1 = i;
                } else if (dist < minDist2) {
                    minDist2 = dist;
                    idx2 = i;
                }
            }
            // Ensure idx1 < idx2 for consistency
            if (idx1 > idx2) std::swap(idx1, idx2);
            nearestPointsIndices[0] = static_cast<int8_t>(idx1);
            nearestPointsIndices[1] = static_cast<int8_t>(idx2);
        }

        void calculateBlendFactor(){
            // Calculate blendFactor between the two nearest points
            float p0 = animationPoints[nearestPointsIndices[0]];
            float p1 = animationPoints[nearestPointsIndices[1]];
            if (std::abs(p1 - p0) > 1e-5f) {
                blendFactor = std::abs(animationPointState - p0) / std::abs(p1 - p0);
            } else {
                blendFactor = 0.0f; // Avoid division by zero
            }
        }
    };

    struct Transition{
        std::string fromState;
        std::string toState;

        float time = 0.0f;
        float ticksPerSecond = 1.0f;
        float speed = 1.0f;
        float duration = 1.0f;
    };

    struct ActionAnimationBlend{
        std::string actionState;
        std::vector<uint32_t> nodeMask;
        std::string baseState;
        float time = 0.0f;
        float blendInDuration = 0.25f;
        float blendOutDuration = 0.25f;
    };

    struct UpdateVisitorBase{
        flecs::entity skeletonInstance;
        
        uint32_t getUpperKeyFrameIndex(const std::vector<float>& keyFrameTimes, const float animationTime);

        float getScaleFactor(const float lowerKeyFrameTime, const float upperKeyFrameTime, const float animationTime);
        
        std::vector<KeyFrame> interpolateKeyFrames(const std::vector<KeyFrame>& lowerKeyFrames, const std::vector<KeyFrame>& upperKeyFrames, float scaleFactor);

        
        std::vector<KeyFrame> getPose(AnimationClip& animation, const float animationTime, const float ticksPerSecond, const float animationSpeed, const float duration);
    };

    struct UpdateVisitor : public UpdateVisitorBase {
        AnimationStateMachine& stateMachine;

        std::vector<KeyFrame> operator()(SingleAnimation& state);
        std::vector<KeyFrame> operator()(BlendSpace1D& state);
        std::vector<KeyFrame> operator()(Transition& state);
        std::vector<KeyFrame> operator()(ActionAnimationBlend& state);
    private:
        std::vector<KeyFrame> getAndUpdatePoseVariant(std::variant<SingleAnimation, BlendSpace1D, Transition, ActionAnimationBlend>& state);

        std::vector<KeyFrame> handleState(SingleAnimation& state);

        std::vector<KeyFrame> handleState(BlendSpace1D& state);

        std::vector<KeyFrame> handleState(Transition& state);

        std::vector<KeyFrame> handleState(ActionAnimationBlend& state);
    };


    struct AnimationStateMachine {
        std::string currentStateName; 
        
        std::map<std::string, 
        std::variant<
            SingleAnimation,
            BlendSpace1D,
            Transition,
            ActionAnimationBlend
        >> animationStates;

        void addAnimationState(const std::string& name, const std::variant<SingleAnimation, BlendSpace1D, Transition, ActionAnimationBlend>& state);

        void setCurrentState(const std::string& name);
        void transitionToState(const std::string& name, float duration = 0.25f);

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

        
        inline std::vector<KeyFrame> UpdateVisitorBase::getPose(AnimationClip& animation, const float animationTime, const float ticksPerSecond, const float animationSpeed, const float duration){
            const auto& keyFrameTimes = animation.keyFrameTimes;
            uint32_t keyFrameIndex = getUpperKeyFrameIndex(keyFrameTimes, animationTime);
            keyFrameIndex = keyFrameIndex * (keyFrameIndex > 0) + 1 * (keyFrameIndex == 0);

            const auto& upperKeyFrames = animation.keyFrames[keyFrameIndex];
            const auto& lowerKeyFrames = animation.keyFrames[keyFrameIndex - 1];

            if(keyFrameTimes[keyFrameIndex] < animationTime){
                return upperKeyFrames;
			}
            float scaleFactor = getScaleFactor(keyFrameTimes[keyFrameIndex - 1], keyFrameTimes[keyFrameIndex], animationTime);
            std::vector<KeyFrame> keyFrames = interpolateKeyFrames(lowerKeyFrames, upperKeyFrames, scaleFactor);
            
            // animationTime += ticksPerSecond * animationSpeed * Time::deltaTime;
            // animationTime = fmod(animationTime, duration);

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

        inline std::vector<KeyFrame> UpdateVisitor::operator()(ActionAnimationBlend& state) {
            std::vector<KeyFrame> finalePose = handleState(state);

            auto& stateActionVariant = stateMachine.animationStates.at(state.actionState);
            if(auto* pSingleAnimation = std::get_if<SingleAnimation>(&stateActionVariant)){
                if(pSingleAnimation->animationTime >= pSingleAnimation->duration){
                    pSingleAnimation->animationTime = 0.0f; // Reset action animation time
                    stateMachine.setCurrentState(state.baseState);
                    state.time = 0.0f; // Reset blend time
                }
            }else{
                RX_LOGE("UpdateVisitor", "operator(ActionAnimationBlend)", "state.actionState only implemented for SingleAnimation");
            }
            return finalePose;
        }

        inline std::vector<KeyFrame> UpdateVisitor::getAndUpdatePoseVariant(std::variant<SingleAnimation, BlendSpace1D, Transition, ActionAnimationBlend>& state) {
            return std::visit([this](auto& specificState) -> std::vector<KeyFrame> {
                return this->handleState(specificState);
            }, state);
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(SingleAnimation& state) {
            std::vector<KeyFrame> keyFrames =  getPose(*state.animation, state.animationTime, state.ticksPerSecond, state.animationSpeed, state.duration);
            state.animationTime += state.ticksPerSecond * state.animationSpeed * Time::deltaTime;
            //state.animationTime = fmod(state.animationTime, state.duration);
            return keyFrames;
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(BlendSpace1D& state) {            
            state.calculateNearestAnimationPoints();
            state.calculateBlendFactor();
            std::vector<KeyFrame> keyFrames1 = 
            getPose
            (*state.animations[state.nearestPointsIndices[0]], state.animationTime * state.duration[state.nearestPointsIndices[0]], 
            state.ticksPerSecond[state.nearestPointsIndices[0]], state.animationSpeed[state.nearestPointsIndices[0]], 
            state.duration[state.nearestPointsIndices[0]]);
            std::vector<KeyFrame> keyFrames2 = 
            getPose
            (*state.animations[state.nearestPointsIndices[1]], state.animationTime * state.duration[state.nearestPointsIndices[1]], 
            state.ticksPerSecond[state.nearestPointsIndices[1]], state.animationSpeed[state.nearestPointsIndices[1]],
            state.duration[state.nearestPointsIndices[1]]);

            auto keyFrames =  interpolateKeyFrames(keyFrames1, keyFrames2, state.blendFactor);

            float durationInSecondState1 = state.duration[state.nearestPointsIndices[0]] / (state.ticksPerSecond[state.nearestPointsIndices[0]] * state.animationSpeed[state.nearestPointsIndices[0]]);
            float durationInSecondState2 = state.duration[state.nearestPointsIndices[1]] / (state.ticksPerSecond[state.nearestPointsIndices[1]] * state.animationSpeed[state.nearestPointsIndices[1]]);

            float blendedTime =  durationInSecondState1 * (1.0f - state.blendFactor) + durationInSecondState2 * state.blendFactor;
            state.animationTime = fmod(state.animationTime + Time::deltaTime/blendedTime, 1.f);


            return keyFrames;
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(Transition& state) {
            auto& fromStateVariant = stateMachine.animationStates.at(state.fromState);
            auto& toStateVariant = stateMachine.animationStates.at(state.toState);

            std::vector<KeyFrame> keyFramesFrom = getAndUpdatePoseVariant(fromStateVariant);
            std::vector<KeyFrame> keyFramesTo = getAndUpdatePoseVariant(toStateVariant);
            
            float blendFactor = std::min(1.0f, state.time / state.duration);
            std::vector<KeyFrame> finalKeyFrames = interpolateKeyFrames(keyFramesFrom, keyFramesTo, blendFactor);

            state.time += state.ticksPerSecond * state.speed * Time::deltaTime;
            return finalKeyFrames;
        }

        inline std::vector<KeyFrame> UpdateVisitor::handleState(ActionAnimationBlend& state){
            auto& stateActionVariant = stateMachine.animationStates.at(state.actionState);
            auto& stateBaseVariant = stateMachine.animationStates.at(state.baseState);

            // This assumes the action is a SingleAnimation to get its duration.
            float actionDuration = 0.0f;
            if(auto* pSingleAnimation = std::get_if<SingleAnimation>(&stateActionVariant)){
                actionDuration = pSingleAnimation->duration / pSingleAnimation->ticksPerSecond;
            }else{
                RX_LOGE("UpdateVisitor", "handleState", "Action state is not a SingleAnimation");
            }

           // Calculate blend factor
            float blendFactor = 0.0f;
            if (state.time < state.blendInDuration) {
                // Blend In
                blendFactor = state.time / state.blendInDuration;
            }
            else if (actionDuration > 0 && state.time > actionDuration - state.blendOutDuration) {
                // Blend Out
                float timeInBlendOut = state.time - (actionDuration - state.blendOutDuration);
                blendFactor = 1.0f - (timeInBlendOut / state.blendOutDuration);
            }
            else {
                // Fully blended
                blendFactor = 1.0f;
            }
            blendFactor = glm::clamp(blendFactor, 0.0f, 1.0f);

            std::vector<KeyFrame> keyFramesAction = getAndUpdatePoseVariant(stateActionVariant);
            std::vector<KeyFrame> keyFramesBase = getAndUpdatePoseVariant(stateBaseVariant);

            // Start with the base pose
            std::vector<KeyFrame> finalKeyFrames = keyFramesBase;

            // Blend only the masked nodes
            for (const auto& nodeIndex : state.nodeMask) {
                const KeyFrame& baseKeyFrame = keyFramesBase[nodeIndex];
                const KeyFrame& actionKeyFrame = keyFramesAction[nodeIndex];
                KeyFrame& finalKeyFrame = finalKeyFrames[nodeIndex];

                finalKeyFrame.position = glm::mix(baseKeyFrame.position, actionKeyFrame.position, blendFactor);
                finalKeyFrame.rotation = glm::slerp(baseKeyFrame.rotation, actionKeyFrame.rotation, blendFactor);
                finalKeyFrame.scaling = glm::mix(baseKeyFrame.scaling, actionKeyFrame.scaling, blendFactor);
            }

            state.time += Time::deltaTime;
            return finalKeyFrames;
        }

        inline void AnimationStateMachine::addAnimationState(const std::string& name, const std::variant<SingleAnimation, BlendSpace1D, Transition, ActionAnimationBlend>& state) {
            animationStates[name] = state;
        }

        inline void AnimationStateMachine::setCurrentState(const std::string& name) {
            auto it = animationStates.find(name);
            if (it != animationStates.end()) {
                currentStateName = name; // Keep the name in sync
                if (std::holds_alternative<Transition>(it->second)) {
                    // Reset transition time when setting it directly
                    std::get<Transition>(it->second).time = 0.0f;
                }
            } else {
                RX_LOGE("AnimationStateMachine", "setCurrentState", 
                        ("Animation state '" + name + "' not found").c_str());
            }
        }

        inline void AnimationStateMachine::update(flecs::entity skeletonInstance, KeyFrameBuffer& keyFrameBuffer){
            UpdateVisitor visitor{skeletonInstance, *this};
            std::vector<KeyFrame> keyFrames = std::visit(visitor, animationStates.at(currentStateName));
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

        inline void AnimationStateMachine::transitionToState(const std::string& toStateName, float duration) {
            if (animationStates.find(toStateName) == animationStates.end()) {
                RX_LOGE("AnimationStateMachine", "transitionToState", ("Destination state '" + toStateName + "' not found.").c_str());
                return;
            }

            // Don't create a transition if we are already in the target state.
            if (currentStateName == toStateName) {
                return;
            }

            // Create a unique name for the new transition state.
            std::string transitionName = currentStateName + "_to_" + toStateName;

            // Create the new transition state object.
            Transition newTransition;
            newTransition.fromState = currentStateName;
            newTransition.toState = toStateName;
            newTransition.duration = duration;
            newTransition.time = 0.0f; // Start the transition from the beginning.

            // Add the new state to the map and set it as the current state.
            animationStates[transitionName] = newTransition;
            setCurrentState(transitionName);
        }
}
}