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
    }
}