#include "CharacterCapsule.hpp"

namespace Rx::Component
{
    void CharacterCapsule::turn(float direction) {
        rotation = rotation * JPH::Quat::sRotation(JPH::Vec3(0, 1, 0), direction * turnSpeed);
    }

  
    void CharacterCapsule::move(const glm::vec3 direction) {
        velocity = Core::toJoltVec3(direction) * moveSpeed;
    }

    void CharacterCapsule::jump() {
        if (character && character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
            gravityVelocity += JPH::Vec3(0.0f, jumpSpeed, 0.0f);
			startJumping = true;
        }
    }
    
void CharacterCapsule::incrSpeed() {
        float targetSpeed = isRunning ? maxRunSpeed : maxWalkSpeed;
        moveSpeed = std::min(moveSpeed + speedIncrement * Time::deltaTime, targetSpeed);
    }

    void CharacterCapsule::decrSpeed() {
        float targetSpeed = isRunning ? maxWalkSpeed : minMoveSpeed;
        if (moveSpeed > targetSpeed) {
            moveSpeed = std::max(moveSpeed - speedDecrement * Time::deltaTime, targetSpeed);
        }
    }

    void CharacterCapsule::update(){
        if(character->GetGroundState() != JPH::CharacterVirtual::EGroundState::OnGround){
            gravityVelocity += JPH::Vec3(0.0f, -9.81f, 0.0f) * Time::deltaTime;
        }else if(!startJumping){
            gravityVelocity = JPH::Vec3::sZero();
        }else {
			startJumping = false;
        }
        character->SetLinearVelocity(velocity + gravityVelocity);
        character->SetRotation(rotation);
    }

    void CharacterCapsule::updateTransform(Transform& transform) {
        transform.translation = Core::toGlmVec3(character->GetPosition());
        auto rotation = Core::toGlmQuat(character->GetRotation());
        transform.angle = glm::angle(rotation);
        transform.axis = glm::axis(rotation);
    }
}