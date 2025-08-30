#pragma once

#include "CharacterCapsule.hpp"
#include "AnimationStateMachine.hpp"

namespace Rx::Component
{
    struct CharacterController
    {
        bool isIdle = true;
        bool isWalking = false;
        bool isRunning = false;
        bool onGround = false;
        bool isJumping = false;
        bool isAttacking = false;

        void initASM(flecs::entity e, AnimationStateMachine& stateMachine);
        void update(CharacterCapsule& capsule, AnimationStateMachine& stateMachine, Transform& transform);
    };



    void CharacterController_on_add(flecs::entity e, AnimationStateMachine& animationStateMachine, CharacterController& controller);
}