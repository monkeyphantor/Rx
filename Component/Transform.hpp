#pragma once

#include "../Core/Global.hpp"

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

            Transform operator*(const Transform& other) const
            {
                Transform result;

                // Combine rotations
                glm::quat q1 = toRotation();
                glm::quat q2 = other.toRotation();
                glm::quat q_result = q1 * q2;

                // Extract angle/axis from combined rotation
                result.angle = glm::angle(q_result);
                result.axis = glm::axis(q_result);
                if (glm::length(result.axis) < glm::epsilon<float>()) {
                    result.axis = glm::vec3(0.0f, 1.0f, 0.0f); // Default axis
                }

                // Combine scales
                result.scale = scale * other.scale;

                // Combine translations
                // The new translation is the translation of the first transform
                // plus the scaled and rotated translation of the second transform.
                glm::vec3 scaled_translated_p2 = q1 * (scale * other.translation);
                result.translation = translation + scaled_translated_p2;

                return result;
            }

            static Transform fromGlmMat4(const glm::mat4& matrix)
            {
                Transform transform;
                // Extract translation
                transform.translation = glm::vec3(matrix[3]);

                // Extract scale factors from the column vectors
                transform.scale.x = glm::length(glm::vec3(matrix[0]));
                transform.scale.y = glm::length(glm::vec3(matrix[1]));
                transform.scale.z = glm::length(glm::vec3(matrix[2]));

                // Build a normalized rotation matrix
                glm::mat3 rotMat;
                rotMat[0] = glm::vec3(matrix[0]) / transform.scale.x;
                rotMat[1] = glm::vec3(matrix[1]) / transform.scale.y;
                rotMat[2] = glm::vec3(matrix[2]) / transform.scale.z;

                // Convert to quaternion
                glm::quat q = glm::quat_cast(rotMat);

                // Extract angle/axis from quaternion
                transform.angle = glm::angle(q);
                transform.axis  = glm::axis(q);

                // Fallback axis if angle is near zero
                if (glm::length(transform.axis) < glm::epsilon<float>())
                    transform.axis = glm::vec3(0.0f, 1.0f, 0.0f);

                return transform;
            }
        };
    }
}