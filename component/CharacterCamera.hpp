#include "Input.hpp"
#include "Eye.hpp"
#include "Transform.hpp"
#include "LocalTransform.hpp"

namespace Rx
{

    namespace Component
    {

        struct CharacterCamera
        {
        public:
            CharacterCamera() : angle(glm::vec2(0,0)), offset(1.0f), rotationSpeed(1.0f),
            fov(45.0f), nearClip(0.1f), farClip(100.0f)
            {

            }

            CharacterCamera
            (glm::vec2 angle, float offset,float rotationSpeed,
            float fov, float nearClip, float farClip) :
            angle(angle),
            offset(offset), rotationSpeed(rotationSpeed),
            fov(fov), nearClip(nearClip), farClip(farClip)
            {

            }

            glm::vec2 angle;
            float offset;
            float rotationSpeed;
            float fov;
            float nearClip;
            float farClip;

        public:
            void update(LocalTransform& localTransform);

            void updateEye(Transform& transform);
        };
    }
}