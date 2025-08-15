#include "VkTextureMaterialBuffer.hpp"
#include "Material.hpp"

namespace Rx {
    namespace Component {

        void textureMaterialBuffer_component_on_add(flecs::entity e, TextureMaterialBuffer& textureMaterialBuffer, VkTextureMaterialBuffer& vkTextureMaterialBuffer) {
            // Create the texture material buffer with the specified number of materials
            vkTextureMaterialBuffer.buffer = Core::createBufferInterface(
                textureMaterialBuffer.materials.size(),
                sizeof(TextureMaterial),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            );

            vkTextureMaterialBuffer.maxNumberMaterials = textureMaterialBuffer.materials.size();
            vkTextureMaterialBuffer.numberMaterials = textureMaterialBuffer.materials.size();

            memcpy(vkTextureMaterialBuffer.buffer.pMemory, textureMaterialBuffer.materials.data(), vkTextureMaterialBuffer.numberMaterials * sizeof(TextureMaterial));
        }

        void textureMaterialBuffer_component_on_remove(flecs::entity e, VkTextureMaterialBuffer& textureMaterialBuffer) {
            // Cleanup GPU resources
            Core::destroyBufferInterface(textureMaterialBuffer.buffer);
        }
    }
} 