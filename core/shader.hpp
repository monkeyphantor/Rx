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

        inline Shader colorMeshVertexShader("./shader/testShader3.vert.spv");
        inline Shader colorMeshFragmentShader("./shader/testShader3.frag.spv");

        inline Shader colorMeshArrayVertexShader{"shader/colorMeshArrayVertexShader.vert.spv"};
        inline Shader colorMeshArrayFragmentShader{"shader/testShader3.frag.spv"};


    }
}