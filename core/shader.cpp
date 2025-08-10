#include "shader.hpp"
#include "device.hpp"

namespace Rx
{
    namespace Core
    {
        Shader::Shader(std::string filePath)
        {
            std::ifstream file(filePath, std::ios::ate | std::ios::binary);

            if(!file.is_open())
            {
                RX_LOGE("Shader", "failed to open", filePath.data())
            }

            size_t fileSize = static_cast<size_t>(file.tellg());

            code.resize(fileSize);

            file.seekg(0);
            file.read(code.data(), fileSize);
            file.close();
        }

        Shader::Shader(const unsigned char* codeData, size_t codeSize)
        {
            code.assign(reinterpret_cast<const char*>(codeData),
                        reinterpret_cast<const char*>(codeData) + codeSize);
        }
        
        VkShaderModule Shader::createShaderModule()
        {
            VkShaderModule vkShaderModule;

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = static_cast<uint32_t>(code.size());
            createInfo.pCode = reinterpret_cast<uint32_t*>(code.data());
            
            RX_CHECK_VULKAN
            (vkCreateShaderModule
            (vkDevice,
            &createInfo,
            nullptr,
            &vkShaderModule),
            "createShaderModule",
            "vkCreateShaderModule")

            return vkShaderModule;
        }

    }
}