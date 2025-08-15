#pragma once

#include "Global.hpp"

namespace Rx
{
    namespace Component
    {
        struct 
            Velocity
        {
            glm::vec3 velocity;
            glm::vec3 angularVelocity;

            void addVelocity(const glm::vec3 deltaVelocity)
            {
                velocity += deltaVelocity;
            }
        };
    }
}