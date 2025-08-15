#pragma once
#include "../Core/Global.hpp"


namespace Rx{
    namespace Component{
        
        struct Material
        {
            glm::vec3 albedo;       // Base color
            float metallic;         // Metallic factor
            float roughness;        // Roughness factor
            glm::vec3 emissive;
        };

        struct TextureMaterial{
            glm::ivec4 textureIndex;
            glm::vec4 albedo;
            glm::vec4 metalRough;
            glm::vec4 emissive;
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