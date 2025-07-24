#pragma once

#include "global.hpp"

namespace Rx
{
    namespace Core
    {
        struct Shader
        {
        public:
            Shader(std::string filePath);
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

    }
}