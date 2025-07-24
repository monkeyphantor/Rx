#pragma once


 #define RX_DEBUG

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


//C
#include <string.h>
#include <time.h>
//C++
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <bitset>
#include <memory>
#include <thread>
#include <random>
#include <functional>
#include <typeinfo>
#include <algorithm>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/quaternion.hpp"

#define RX_LOGI(X, Y, Z)\
{\
    printf("Info@RX: %s :: %s :: %s\n", X, Y, Z);\
}

#define RX_LOGE(X, Y, Z)\
{\
    printf("Error@RX: %s :: %s :: %s\n", X, Y, Z);\
    exit(1);\
}

#define RX_CHECK_VULKAN(VulkanExpression, Function, VulkanFunction)\
{\
    VkResult vkRes;\
    if((vkRes = VulkanExpression) != VK_SUCCESS)\
    {\
        printf("Error@RX: %s :: %s\n", Function, VulkanFunction);\
        printf("Vulkan Error Code: %d\n", vkRes);\
        exit(1);\
    }\
}


#define RX_ASSERT(Expression, Class, Funktion, Message)\
{\
    if(!(Expression))\
    {\
        RX_LOGE(Class, Funktion, Message)\
    }\
}

