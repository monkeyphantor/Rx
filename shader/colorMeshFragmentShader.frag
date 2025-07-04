#version 450
#extension GL_EXT_nonuniform_qualifier : enable

struct PointLight {
    vec4 position;
    vec4 color;
    vec4 intensity;
};

struct DirectionalLight{
    vec4 direction;
    vec4 color;
    vec4 intensity;
};

struct Material {
    ivec4 textureIndex;
    vec4 albedo;
    vec4 metalRough;
    vec4 emissive;
};

layout(binding = 0) uniform Eye {
    vec4 position;
    vec4 direction;
    mat4 projView;
} eye;

layout(binding = 1) uniform PointLightBuffer {
    vec4 numberPointLights;
    PointLight lights[256];
} pointLightBuffer;

layout(binding = 2) uniform DirectionalLightBuffer{
    vec4 numberDirectionalLights;
    DirectionalLight lights[256];
}directionalLightBuffer;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 albedo;
layout(location = 3) in vec3 metalRough;
layout(location = 4) in vec3 emissive;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;


float GGX(float NdotH, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    return a2/max(denom, 0.00000001);
}

float geometrySmith(float NdotV, float NdotL, float roughness){
    float r = roughness + 1.0;
    float k = (r*r)/8.0;
    float ggx1 = NdotV/(NdotV * (1.0 - k) + k);
    float ggx2 = NdotL/(NdotL * (1.0 - k) + k);

    return ggx1 * ggx2; 
}

vec3 fresnelSchlick(float HdotV, vec3 baseReflectivity){
    return baseReflectivity +  (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
}


vec3 PBR_PointLight(vec3 V, vec3 N, vec3 baseReflectivity, vec3 fragPosition, vec3 eyePosition, vec3 lightPosition, vec3 lightColor, float intensity, vec3 albedo, vec3 emissive, float roughness, float metallic){
    
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 H = normalize(V+L);
    float dist = length(lightPosition - fragPosition);
    float attenuation = 1.0/(dist*dist);
    vec3 radiance = lightColor * intensity * attenuation;

    float NdotV = max(dot(N,V), 0.0000001);
    float NdotL = max(dot(N,L), 0.0000001);
    float HdotV = max(dot(H,V), 0.0000001);
    float NdotH = max(dot(N,H), 0.0000001);

    float D = GGX(NdotH, roughness);
    float G = geometrySmith(NdotV, NdotL, roughness);
    vec3 F = fresnelSchlick(HdotV, baseReflectivity);

    vec3 specular = D*G*F;
    specular /= 4.0 * NdotV * NdotL;
    vec3 kD = vec3(1.0) - F;

    kD *= 1.0 - metallic;

    return (kD*albedo/PI + specular) * radiance * NdotL;

}

vec3 PBR_DirectionalLight(vec3 V, vec3 N, vec3 baseReflectivity, vec3 fragPosition, vec3 eyePosition, vec3 lightDirection, vec3 lightColor, float intensity, vec3 albedo, vec3 emissive, float roughness, float metallic){
    
    vec3 L = normalize(-lightDirection);
    vec3 H = normalize(V+L);
    vec3 radiance = lightColor * intensity;

    float NdotV = max(dot(N,V), 0.0000001);
    float NdotL = max(dot(N,L), 0.0000001);
    float HdotV = max(dot(H,V), 0.0000001);
    float NdotH = max(dot(N,H), 0.0000001);

    float D = GGX(NdotH, roughness);
    float G = geometrySmith(NdotV, NdotL, roughness);
    vec3 F = fresnelSchlick(HdotV, baseReflectivity);

    vec3 specular = D*G*F;
    specular /= 4.0 * NdotV * NdotL;
    vec3 kD = vec3(1.0) - F;

    kD *= 1.0 - metallic;

    return (kD*albedo/PI + specular) * radiance * NdotL;

}

void main() {



    float metallic = metalRough[0];
    float roughness = metalRough[1];

    vec3 N = normalize(fragNormal); 
    vec3 V = normalize(eye.position.xyz - fragPosition);

    vec3 baseReflectivity = mix(vec3(0.04), albedo,  metallic);

    vec3 Lo = vec3(0.0); // Accumulated radiance

    for (int i = 0; i < int(pointLightBuffer.numberPointLights.x); ++i) {
        PointLight light = pointLightBuffer.lights[i];

        Lo += PBR_PointLight(V, N, baseReflectivity, fragPosition, eye.position.xyz, light.position.xyz, light.color.xyz, light.intensity.x, albedo, emissive, roughness, metallic);
    }

    for (int i = 0; i < int(directionalLightBuffer.numberDirectionalLights.x); ++i) {
        DirectionalLight light = directionalLightBuffer.lights[i];

        Lo += PBR_DirectionalLight(V, N, baseReflectivity, fragPosition, eye.position.xyz, light.direction.xyz, light.color.xyz, light.intensity.x, albedo, emissive, roughness, metallic);
    }

    vec3 ambient = vec3(0.03) * albedo;
    vec3 color =  Lo + emissive + ambient;

    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));
    
    outColor = vec4(color, 1.0);
}