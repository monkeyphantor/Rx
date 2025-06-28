#version 460
#extension GL_ARB_shader_draw_parameters : require


layout(binding = 0) uniform Eye
{
    mat4 projView;
} eye;

struct ColorMeshInstance
{
    mat4 transform;
};

layout(binding = 1, std430) restrict readonly buffer ModelBuffer
{
    ColorMeshInstance instances[];
} modelBuffer;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 colorOut;

void main() {
    // Use gl_DrawID to index into the correct instance transform
    mat4 instanceTransform = modelBuffer.instances[gl_DrawID].transform;
    
    gl_Position = eye.projView * instanceTransform * vec4(position, 1.0);
    
    colorOut = color;
}