#pragma once

#include "../Core/Global.hpp"

namespace Rx
{
    namespace Component
    {
        struct LocalTransform {

            glm::vec3 scale; 
            float angle; 
            glm::vec3 axis;
            glm::vec3 translation;


            // Decomposes a matrix into scale, rotation, and translation.
            void fromGlmMat4(const glm::mat4& matrix)
            {
                // Extract translation from the 4th column
                translation = glm::vec3(matrix[3]);

                // Extract scale from the length of the matrix columns
                scale.x = glm::length(glm::vec3(matrix[0]));
                scale.y = glm::length(glm::vec3(matrix[1]));
                scale.z = glm::length(glm::vec3(matrix[2]));

                // Avoid division by zero if scale is zero
                if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) {
                    angle = 0.0f;
                    axis = glm::vec3(0.0f, 1.0f, 0.0f);
                    return;
                }

                // Build a rotation matrix by normalizing the columns
                const glm::mat3 rotationMatrix = glm::mat3(
                    glm::vec3(matrix[0]) / scale.x,
                    glm::vec3(matrix[1]) / scale.y,
                    glm::vec3(matrix[2]) / scale.z
                );

                // Convert rotation matrix to quaternion, then to angle/axis
                glm::quat rotation = glm::quat_cast(rotationMatrix);
                angle = glm::angle(rotation);
                axis = glm::axis(rotation);
            }

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