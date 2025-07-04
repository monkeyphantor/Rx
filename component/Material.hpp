#pragma once
#include "../core/global.hpp"


namespace Rx{
    namespace Component{
        
        struct Material
        {
            glm::vec3 albedo;       // Base color
            float metallic;         // Metallic factor
            float roughness;        // Roughness factor
            glm::vec3 emissive;
        };


        struct SquareMaterial {
            glm::vec3 albedo;       // Base color
            float metallic;         // Metallic factor
            float roughness;        // Roughness factor
            glm::vec3 emissive;
            glm::vec3 lightColor;
            float lightIntensity;
        };
    }
}