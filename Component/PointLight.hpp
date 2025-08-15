#pragma once

#include "../Core/Light.hpp"
#include "Transform.hpp"

namespace Rx{
    namespace Component{

        struct PointLight{

            public:
            glm::vec3 color;
            float intensity;

            void copyTo(Transform& transform, Core::PointLight& other){
                other.position = glm::vec4(transform.translation, 1.f);
                other.color = glm::vec4(color, 1.f);
                other.intensity = glm::vec4(intensity, 0.f,0.f,0.f);
            }
        
        };
    }
}