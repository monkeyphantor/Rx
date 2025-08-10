#pragma once

#include "global.hpp"
#include <cstddef>

namespace Rx
{
    namespace Core
    {
        struct Shader
        {
        public:
            Shader(std::string filePath);
            Shader(const unsigned char* codeData, size_t codeSize);
            ~Shader()  = default;

        private:
            std::vector<char> code{};

        public:
            VkShaderModule createShaderModule();
        };

        inline Shader colorMeshVertexShader("./shader/colorMeshVertexShader.vert.spv");
        inline Shader colorMeshFragmentShader("./shader/colorMeshFragmentShader.frag.spv");

        inline Shader colorMeshArrayVertexShader{"shader/colorMeshArrayVertexShader.vert.spv"};
        inline Shader colorMeshArrayFragmentShader{"shader/colorMeshFragmentShader.frag.spv"};

        inline Shader instancedColorMeshVertexShader{"shader/instancedColorMeshVertexShader.vert.spv"};
        inline Shader instancedColorMeshFragmentShader{"shader/colorMeshFragmentShader.frag.spv"};

        inline Shader textureModelVertexShader{"shader/TextureModelShader.vert.spv"};
        inline Shader textureModelFragmentShader{"shader/TextureModelShader.frag.spv"};

        inline Shader skeletonModelVertexShader{"shader/SkeletonModelShader.vert.spv"};
        inline Shader skeletonModelFragmentShader{"shader/SkeletonModelShader.frag.spv"};

#include "../shader/KeyframeToNode.comp.spv.h"
        inline Shader keyframeToNodeShader{shader_KeyframeToNode_comp_spv, shader_KeyframeToNode_comp_spv_len};
    }
}
