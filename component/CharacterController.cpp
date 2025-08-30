#include "CharacterController.hpp"
#include "AnimationMap.hpp"
#include "AnimationClip.hpp"
#include "Input.hpp"

namespace Rx::Component
{
    void CharacterController_on_add(flecs::entity e, AnimationStateMachine& animationStateMachine, CharacterController& controller)
    {
        controller.initASM(e, animationStateMachine);
    }


    void CharacterController::initASM(flecs::entity e, AnimationStateMachine& stateMachine)
    {
        auto& animationMap = e.get<Rx::Component::AnimationMap>();
        auto& animations = animationMap.animations;
        auto& boneIndices = animationMap.boneIndices;
        auto* pAnimClipIdle = animations.at("Idle").get();
        auto* pAnimClipWalk = animations.at("Walk").get();
        auto* pAnimClipRun = animations.at("Run").get();

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
    }
    
    void CharacterController::update(CharacterCapsule& capsule, AnimationStateMachine& stateMachine, Transform& transform)
    {
        if(Input::keyA.down){
            capsule.turn(Time::deltaTime);
        }
        if(Input::keyD.down){
            capsule.turn(-Time::deltaTime);
        }

        if(Input::keyW.down){
            capsule.isRunning = Input::keyShift.down;
            if(!capsule.isRunning && capsule.moveSpeed > capsule.maxWalkSpeed){
                capsule.decrSpeed();
            }else{
                capsule.incrSpeed();
            }
        }else{
            capsule.isRunning = false;
            capsule.decrSpeed();
        }

        capsule.move(transform.forward());

        if(Input::keySpace.down){
            capsule.jump();
        }

        if(auto* blendSpace = std::get_if<Rx::Component::BlendSpace1D>(&stateMachine.animationStates["IdleWalkRun"]))
        {
            blendSpace->animationPointState = capsule.moveSpeed/capsule.maxRunSpeed;
        }

        if(Input::buttonLeft.down && stateMachine.currentStateName != "Attack"){
            stateMachine.setCurrentState("Attack");
        }

    }

}
