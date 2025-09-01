#include "NpcController.hpp"
#include "AnimationMap.hpp"
#include "AnimationClip.hpp"
#include "Input.hpp"

namespace Rx::Component
{
    void NpcController_on_add(flecs::entity e, AnimationStateMachine& animationStateMachine, NpcController& controller)
    {
        controller.initASM(e, animationStateMachine);
    }


    void NpcController::initASM(flecs::entity e, AnimationStateMachine& stateMachine)
    {
        auto& animationMap = e.get<Rx::Component::AnimationMap>();
        auto& animations = animationMap.animations;
        auto& boneIndices = animationMap.boneIndices;
        auto* pAnimClipIdle = animations.at("Idle").get();
        auto* pAnimClipWalk = animations.at("Walk").get();
        auto* pAnimClipRun = animations.at("Run").get();
        auto* pAnimClipDeath = animations.at("Death").get();

        stateMachine.addAnimationState("IdleWalkRun", 
            Rx::Component::BlendSpace1D
            {.animations = {pAnimClipIdle, pAnimClipWalk, pAnimClipRun},
			.numberAnimationPoints = 3,
            .animationPoints = {0.0f, 0.5f, 1.0f},
            .animationPointState = 0.0f,
            .blendFactor = 0.0f,
            .animationTime = 0.0f,
            .ticksPerSecond = {pAnimClipIdle->ticksPerSecond, pAnimClipWalk->ticksPerSecond, pAnimClipRun->ticksPerSecond},
            .animationSpeed = {1.0f, 2.0f, 1.0f},
            .duration = {pAnimClipIdle->duration, pAnimClipWalk->duration, pAnimClipRun->duration}
        });
        stateMachine.setCurrentState("IdleWalkRun");

        auto* pAnimClipSpell2 = animations.at("Spell2").get();

        stateMachine.addAnimationState("Spell2",
            Rx::Component::SingleAnimation
            {.animation = pAnimClipSpell2,
            .animationTime = 0.0f,
            .animationSpeed = 1.0f,
            .duration = pAnimClipSpell2->duration,
            .ticksPerSecond = pAnimClipSpell2->ticksPerSecond}
        );

        stateMachine.addAnimationState("Attack",
            Rx::Component::ActionAnimationBlend
            {.actionState = "Spell2",
            .nodeMask = {boneIndices.at("Torso"), 
                            boneIndices.at("Neck"), 
                                boneIndices.at("Head"), 
                            boneIndices.at("Shoulder.L"),
                                boneIndices.at("UpperArm.L"),
                                    boneIndices.at("LowerArm.L"),
                                        boneIndices.at("Fist.L"),
                                            boneIndices.at("Fist1.L"),
                                                boneIndices.at("Fist2.L"),
                                        boneIndices.at("Thumb1.L"),
                                            boneIndices.at("Thumb2.L"),
                            boneIndices.at("Shoulder.R"),
                                boneIndices.at("UpperArm.R"),
                                    boneIndices.at("LowerArm.R"),
                                        boneIndices.at("Fist.R"),
                                            boneIndices.at("Fist1.R"),
                                                boneIndices.at("Fist2.R"),
                                                boneIndices.at("Weapon.R"),
                                        boneIndices.at("Thumb1.R"),
                                            boneIndices.at("Thumb2.R"),
                                   },
            .baseState = "IdleWalkRun",
            .time = 0.0f,
            .blendInDuration = 0.25f,
            .blendOutDuration = 0.25f}
        );


        stateMachine.addAnimationState("Death",
            Rx::Component::SingleAnimation
            {.animation = pAnimClipDeath,
            .animationTime = 0.0f,
            .animationSpeed = 1.0f,
            .duration = pAnimClipDeath->duration,
            .ticksPerSecond = pAnimClipDeath->ticksPerSecond}
        );

    }
    
    void NpcController::update(CharacterCapsule& capsule, AnimationStateMachine& stateMachine, Transform& transform, Transform& playerTransform, FireballNpcLauncher& fireballLauncher)
    {
       // AI parameters
        const float attackRange = 15.0f;
        const float followRange = 20.0f;
        const float stopDistance = 2.0f;

        // Calculate direction and distance to the player on the horizontal plane
        glm::vec3 directionToPlayer = playerTransform.translation - transform.translation;
        directionToPlayer.y = 0; // We only care about horizontal distance and direction
        float distanceToPlayer = glm::length(directionToPlayer);

        // Normalize the direction vector
        if (distanceToPlayer > 0.01f) {
            directionToPlayer = glm::normalize(directionToPlayer);
        }

        // Don't do anything if dead or dying
        const std::string& currentState = stateMachine.currentStateName;
        if (currentState == "Death" || currentState.find("_to_Death") != std::string::npos) {
            capsule.decrSpeed();
            capsule.move({0,0,0});
            return;
        }

        // --- ATTACK LOGIC ---
        // If in attack range and not already attacking, start an attack
        if (distanceToPlayer <= attackRange && stateMachine.currentStateName != "Attack") {
            stateMachine.setCurrentState("Attack");
            fireballLauncher.shouldShoot = true; // Signal to shoot a fireball
        }

        // --- TURNING LOGIC ---
        // Only turn if not in the middle of an attack
        glm::vec3 currentForward = transform.forward();
        float dot = glm::dot(currentForward, directionToPlayer);

        // Check if we need to turn (if we're not already facing the player)
        if (dot < 0.99f) {
            glm::vec3 cross = glm::cross(currentForward, directionToPlayer);
            float turnDirection = (cross.y < 0) ? -1.0f : 1.0f;
            capsule.turn(turnDirection * Time::deltaTime);
        }
    
        // --- MOVEMENT LOGIC ---
        // Move if outside the stopping distance but inside the follow range, and not attacking
        if (distanceToPlayer > stopDistance && distanceToPlayer < followRange && stateMachine.currentStateName != "Attack") {
            capsule.isRunning = true;
            capsule.incrSpeed();
            capsule.move(transform.forward());
        } else {
            // Otherwise, stop moving
            capsule.isRunning = false;
            capsule.decrSpeed();
            // Call move with forward, but speed will be 0 or decreasing
            capsule.move(transform.forward());
        }

        // --- ANIMATION LOGIC ---
        // Update the blend space for walking/running based on current speed
        if(auto* blendSpace = std::get_if<Rx::Component::BlendSpace1D>(&stateMachine.animationStates["IdleWalkRun"]))
        {
            blendSpace->animationPointState = capsule.moveSpeed / capsule.maxRunSpeed;
        }
    }

}
