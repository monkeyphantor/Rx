#pragma once

#include "../core/global.hpp"

namespace Rx
{
    namespace Component
    {
        struct Transform {

            glm::vec3 scale; 
            float angle; 
            glm::vec3 axis;
            glm::vec3 translation;


            glm::mat4 getTransformMatrix() const
            {
                glm::mat4 transform = glm::scale(glm::mat4(1.f), scale);
                transform = glm::rotate(transform, angle, axis);
                transform[3][0] += translation[0];
                transform[3][1] += translation[1];
                transform[3][2] += translation[2];

                return transform;
            }

            glm::quat toRotation() const
            {
                return glm::angleAxis(angle, axis);
            }
        };
    }
}