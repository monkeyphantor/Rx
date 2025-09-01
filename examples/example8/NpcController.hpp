#pragma once

#include "CharacterCapsule.hpp"
#include "AnimationStateMachine.hpp"
#include "FireballNpcLauncher.hpp"

namespace Rx::Component
{
    struct NpcController
    {
        bool isIdle = true;
        bool isWalking = false;
        bool isRunning = false;
        bool onGround = false;
        bool isJumping = false;
        bool isAttacking = false;
        
        void initASM(flecs::entity e, AnimationStateMachine& stateMachine);
        void update(CharacterCapsule& capsule, AnimationStateMachine& stateMachine, Transform& transformm, Transform& playerTransform, FireballNpcLauncher& fireballLauncher);
    };



    void NpcController_on_add(flecs::entity e, AnimationStateMachine& animationStateMachine, NpcController& controller);
}