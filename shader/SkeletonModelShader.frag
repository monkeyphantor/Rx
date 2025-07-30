#version 450
#extension GL_EXT_nonuniform_qualifier : enable

// An infinitely small number to prevent division by zero
#define EPSILON 0.00001

const float PI = 3.14159265359;

// ----- INPUT STRUCTS AND UNIFORMS (from your request) -----

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

layout(std430, binding = 4) buffer MaterialBuffer {
    Material materials[];
};

layout(binding = 6) uniform sampler2D TextureArray[];

// ----- INPUTS FROM VERTEX SHADER -----
// These are interpolated for each fragment.

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in flat int glDrawID;

// ----- OUTPUT -----

layout(location = 0) out vec4 outColor;


// ----- PBR HELPER FUNCTIONS -----

// Trowbridge-Reitz GGX Normal Distribution Function (NDF)
// Estimates the amount of microfacets aligned with the halfway vector.
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, EPSILON);
}

// Schlick-GGX Geometry Function
// Describes the self-shadowing property of the microfacets.
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith's Method for Geometry Function
// Combines the geometry function for both view (G_V) and light (G_L) directions.
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel-Schlick Approximation
// Describes the ratio of surface reflection at a given angle.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


// ----- MAIN SHADER -----

void main()
{

    vec3 albedo;
    Material material = materials[glDrawID];
    if(material.textureIndex.x >= 0) {
        vec4 texAlbedo = texture(TextureArray[nonuniformEXT(material.textureIndex.x)], texCoords);
        if(texAlbedo.a < 0.05){
            discard; // Discard fragments with very low alpha (transparent)
        }
        albedo = texAlbedo.rgb;
    } else {
        albedo = material.albedo.rgb;
    }

    // --- 1. Extract Material Properties ---
    // We assume the .xy components of metalRough hold our PBR values.
    // This is a common packing strategy.
    vec3  baseColor = albedo.rgb;
    float metallic  = material.metalRough.x;
    float roughness = material.metalRough.y;
    vec3 emissive  = material.emissive.rgb;

    // --- 2. Setup Per-Fragment Vectors ---
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(eye.position.xyz - fragPosition);

    // --- 3. Calculate F0 (reflectance at normal incidence) ---
    // For dielectrics (non-metals), F0 is a constant low value.
    // For metals, F0 is the base color itself.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    // --- 4. Main Lighting Calculation ---
    // This will be our accumulated outgoing radiance.
    vec3 Lo = vec3(0.0);

    // --- Directional Lights Loop ---
    for (int i = 0; i < int(directionalLightBuffer.numberDirectionalLights.x); ++i)
    {
        // Light direction is typically "from" the light source. We need the vector *to* the light.
        vec3 L = normalize(-directionalLightBuffer.lights[i].direction.xyz);
        vec3 H = normalize(V + L);
        
        // Calculate radiance (light's color * intensity)
        vec3 radiance = directionalLightBuffer.lights[i].color.rgb * directionalLightBuffer.lights[i].intensity.x;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F; // Specular reflection amount
        // For energy conservation, diffuse reflection is whatever is not reflected specularly.
        vec3 kD = vec3(1.0) - kS;
        // Metals have no diffuse light, so we multiply by (1.0 - metallic)
        kD *= (1.0 - metallic);

        // BRDF Numerator and Denominator
        vec3 numerator   = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + EPSILON;
        vec3 specular    = numerator / denominator;

        // Add to outgoing radiance
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * baseColor / PI + specular) * radiance * NdotL;
    }

    // --- Point Lights Loop ---
    for (int i = 0; i < int(pointLightBuffer.numberPointLights.x); ++i)
    {
        vec3 lightVec = pointLightBuffer.lights[i].position.xyz - fragPosition;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec); // Vector to the light
        vec3 H = normalize(V + L);

        // Attenuation (inverse square law)
        float attenuation = 1.0 / (distance * distance);

        // Calculate radiance
        vec3 radiance = pointLightBuffer.lights[i].color.rgb * pointLightBuffer.lights[i].intensity.x * attenuation;

        // Cook-Torrance BRDF (same as for directional lights)
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        vec3 numerator   = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + EPSILON;
        vec3 specular    = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * baseColor / PI + specular) * radiance * NdotL;
    }

    // --- 5. Ambient and Emissive Light ---
    // A simple ambient term to lift the shadows.
    // For a full PBR pipeline, this would be replaced by Image-Based Lighting (IBL)
    // from a pre-convoluted environment map.

     // Distance-based mix to reduce far-plane flicker
    float viewDist    = length(eye.position.xyz - fragPosition);
    float fadeFactor  = clamp((viewDist - 10000.0)
                             / (20000.0 - 10000.0),
                              0.0, 1.0);
    vec3  mixedLo     = mix(Lo, baseColor, fadeFactor);

    vec3 ambient = vec3(0.03) * baseColor;
    vec3 color = ambient + mixedLo + emissive;

    // --- 6. Post-Processing: HDR to LDR ---
    // Reinhard Tonemapping
    color = color / (color + vec3(1.0));
    // Gamma Correction (assuming sRGB display)
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}