#pragma once

#include "global.hpp"
#include "buffer.hpp"

namespace Rx
{
    namespace Core
    {
        struct Eye
        {
            glm::vec4 position;
            glm::vec4 direction;
            glm::mat4 projView;
        };

        inline BufferInterface eye;

        void createEye();
        void destroyEye();
        void updateEye(glm::vec3 position, glm::vec3 direction, float fov, float nearClip, float farClip);
    }
}