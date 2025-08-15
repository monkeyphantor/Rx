#version 450


layout(binding = 0) uniform Eye
{
    vec4 position;
    vec4 direction;
    mat4 projView;
} eye;


layout(binding = 3) uniform Model
{
    mat4 transform;
    mat4 normalTransform;
    vec4 albedo;
    vec4 metalRough;
    vec4 emissive;
}model;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;


layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 albedo;
layout(location = 3) out vec3 metalRough;
layout(location = 4) out vec3 emissive;

void main() {
    vec4 worldPosition = model.transform * vec4(position, 1.0);
    gl_Position = eye.projView * worldPosition;
    fragPosition = worldPosition.xyz / worldPosition.w;

    fragNormal = normalize(mat3(model.normalTransform) * normal);

    albedo = model.albedo.rgb;
    metalRough = model.metalRough.rgb;
    emissive = model.emissive.rgb;
}