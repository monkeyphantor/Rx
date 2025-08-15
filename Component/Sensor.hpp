#pragma once

#include "Physics.hpp"

namespace Rx {
    namespace Component {

        struct KinematicSensor{
            JPH::ObjectLayer objectLayer;
        };

        struct StaticSensor{
            JPH::ObjectLayer objectLayer;
        };
    }
}