#pragma once

#include "Global.hpp"
#include "flecs.h"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include "Time.hpp"
#include "Physics.hpp"
#include "Transform.hpp"

namespace Rx::Component
{
    struct CharacterCapsule
    {
        JPH::ObjectLayer layer;

        float moveSpeed = 0.0f;    // meters per second
        float turnSpeed = 3.0f;    // radians per second
        float jumpSpeed = 7.0f;
        bool startJumping = false;
        float capsuleHeight = 2.0f;
        float capsuleRadius = 0.5f;
        float speedIncrement = 10.0f;
        float speedDecrement = 10.0f;
        float maxWalkSpeed = 5.0f;
        float maxRunSpeed = 10.0f;
        float minMoveSpeed = 0.0f;
        bool isRunning = false;
        JPH::CharacterVirtual* character = nullptr;

        // Logical state controlled by the CharacterController
        JPH::Quat rotation = JPH::Quat::sIdentity();
        JPH::Vec3 velocity = JPH::Vec3::sZero();
        JPH::Vec3 gravityVelocity = JPH::Vec3::sZero();

        void turn(float direction);

        void move(const glm::vec3 direction);

        void jump();
        
        void incrSpeed();

        void decrSpeed();

        void update();

        void updateTransform(Transform& transform);
    };

}