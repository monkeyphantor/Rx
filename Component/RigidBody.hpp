#pragma once

#include "Physics.hpp"


namespace Rx {
    namespace Component {
        
        struct StaticRigidBody{
            JPH::ObjectLayer objectLayer;
            float friction;
            float restitution;
        };

        struct DynamicRigidBody{
            JPH::ObjectLayer objectLayer;
            float mass;
            float friction;
            float restitution;
        };

        struct KinematicRigidBody{
            JPH::ObjectLayer objectLayer;
            float friction;
            float restitution;
        };
    }
}