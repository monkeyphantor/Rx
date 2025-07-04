#pragma once

#include "../core/Light.hpp"
#include "Transform.hpp"

namespace Rx{
    namespace Component{

        struct DirectionalLight{


            DirectionalLight():
            color(glm::vec3(0.f,0.f,0.f)),
            intensity(0.f){

            }

            DirectionalLight(
            glm::vec3 color,
            float intensity):
            color(color),
            intensity(intensity){

            }

            public:
            glm::vec3 color;
            float intensity;

            void copyTo(Transform& transform, Core::DirectionalLight& other){
                other.direction = glm::vec4(transform.toRotation() * glm::vec3(0.f,0.f,-1.f), 0.f);
                other.color = glm::vec4(color, 1.f);
                other.intensity = glm::vec4(intensity, 0.f,0.f,0.f);
            }
        
        };
    }
}