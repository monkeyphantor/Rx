#include "CharacterCamera.hpp"

namespace Rx::Component
{
    void CharacterCamera::update(LocalTransform& localTransform)
    {
        angle += Input::cursor.deltaPosition * (1.f*Input::buttonRight.down) * rotationSpeed;

        auto rotation = (glm::angleAxis(angle[0], glm::vec3(0.0,-1.0,0.0))*glm::angleAxis(angle[1], glm::vec3(1.0,0.0,0.0)));
        localTransform.angle = glm::angle(rotation);
        localTransform.axis = glm::axis(rotation);

        glm::vec3 direction = rotation * glm::vec3(0.0,0.0,-1.0);

        localTransform.translation = (-direction*offset);
    }

    void CharacterCamera::updateEye(Transform& transform)
    {
        glm::vec3 direction = transform.toRotation() * glm::vec3(0.0,0.0,-1.0);
        Core::updateEye(transform.translation, direction, fov, nearClip, farClip);
    }
}