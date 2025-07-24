#pragma once

#include "global.hpp"
#include "flecs.h"
#include "Material.hpp"

namespace Rx{
    namespace Component{
        struct TextureMaterialBuffer{
            std::vector<TextureMaterial> materials; 

            void addMaterial(uint32_t textureIndex,
                            glm::vec3 albedo,
                            float metallic,
                            float roughness,
                            glm::vec3 emissive)
            {

                glm::vec4 textureIndexVec(textureIndex, 0.0f, 0.0f, 0.0f);
                glm::vec4 albedoVec(albedo, 1.0f);
                glm::vec4 metalRough(metallic, roughness, 0.0f, 0.0f);
                glm::vec4 emissiveVec(emissive, 1.0f);

                materials.push_back({textureIndexVec, albedoVec, metalRough, emissiveVec});
            }

            void addMaterial(const TextureMaterial& material)
            {
                materials.push_back(material);
            }
        };
    }
}