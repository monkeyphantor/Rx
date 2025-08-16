#pragma once
#include "GameWorld.hpp"
#include "Level.hpp"
#include "DirectionalLight.hpp"
#include "VkColorModelBuffer.hpp"
#include "ColorMesh.hpp"
#include "ColorMesh.hpp"
#include "Material.hpp"
#include "Shape.hpp"
#include "FastNoiseLite.h"
#include "VkColorMesh.hpp"
#include "VkInstancedColorModelBuffer.hpp"
#include "VkInstancedColorModelDescriptorSet.hpp"
#define random(lower, upper) ((static_cast<float>(rand())/static_cast<float>(RAND_MAX))*((upper)-(lower)) + (lower))

struct Actors;
struct FireballSystem;
struct Move;
struct LevelManager;
namespace Rx {

    class RxLevel1 : public Rx::Level {
    public:
        // Constructor
        RxLevel1() = default;

        // Destructor
        virtual ~RxLevel1() = default;

        // Called when the level is loaded
        virtual void onLoad(flecs::world& world) override {
            // Implement level-specific loading logic here
                world.import<Actors>();
        }

        // Called when the level is unloaded
        virtual void onUnload(flecs::world& world) override {
            // Implement level-specific unloading logic here
        }
    };

} // namespace Rx


struct Actors {
    // --- NEW ---
    // The master scale constant for the entire world.
    // > 1.0 makes the world larger, < 1.0 makes it smaller.
    const float WORLD_SCALE = 10.0f; 

    // Define some constants for easier tweaking
    const int WORLD_SIZE_X = 250;
    const int WORLD_SIZE_Z = 250;
    const float TERRAIN_HEIGHT_SCALE = 10.0f;
    const float TERRAIN_NOISE_FREQUENCY = 0.05f;
    const float TREE_DENSITY = 0.001f; // 2% chance for a tree to spawn on a block

    // Define block colors
    const glm::vec4 GRASS_COLOR = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
    const glm::vec4 DIRT_COLOR = glm::vec4(0.5f, 0.35f, 0.2f, 1.0f);
    const glm::vec4 TRUNK_COLOR = glm::vec4(0.4f, 0.26f, 0.13f, 1.0f);
    const glm::vec4 LEAVES_COLOR = glm::vec4(0.1f, 0.6f, 0.2f, 1.0f);

    Actors(flecs::world& world) {
        world.module<Actors>();
        setupScene(world);

        // --- Instanced Renderer Setup ---
        // This part is mostly the same, but we use a generic white cube.
        // The color will be set per-instance via the Material component.
        Rx::Shape::ColorCube cube(glm::vec3(1.f), glm::vec4(1.f)); // Base cube is always 1x1x1, it gets scaled by the transform

        auto batchRenderer = world.entity("InstancedRender");
        batchRenderer.set<Rx::Component::ColorMesh>({ cube.getVertices(), cube.getIndices() });
        batchRenderer.add<Rx::Component::VkColorMesh>();

        Rx::Component::VkInstancedColorModelBuffer colorMeshInstanceBuffer;
        colorMeshInstanceBuffer.maxNumberInstances = 1000000;
        batchRenderer.set<Rx::Component::VkInstancedColorModelBuffer>(colorMeshInstanceBuffer);
        batchRenderer.add<Rx::Component::VkInstancedColorModelDescriptorSet>();
        batchRenderer.add<Rx::ShouldBeUpdated>();

        generateWorld(world, batchRenderer);
    }

    // Helper function to create a single block
    // Position is now expected to be in final, scaled world coordinates.
    void createBlock(flecs::world& world, flecs::entity batchRenderer, const glm::vec3& position, const glm::vec4& color) {
        auto rel = world.lookup("InstancedColorMeshRelation");
        auto block = world.entity();
        block.add(rel, batchRenderer);
        block.set<Rx::Component::Transform>({
            // --- MODIFIED ---
            // The scale of the block model itself is now determined by WORLD_SCALE.
            glm::vec3(WORLD_SCALE),     // Scale
            0.f,                        // Angle
            glm::vec3(0.f, 1.f, 0.f),    // Axis
            position                    // Position
        });
        // Set the color via the material. Roughness/metallic can be constant or varied.
        block.set<Rx::Component::Material>({ color, random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
        block.add<LevelAsset>();
    }

void createTree(flecs::world& world, flecs::entity batchRenderer, const glm::vec3& baseGridPos) {
        // Create the trunk (e.g., 4 to 6 blocks high)
        int trunkHeight = 4 + (rand() % 3); // Vary height slightly
        for (int i = 0; i < trunkHeight; ++i) {
            // The offset is in grid units.
            glm::vec3 trunkBlockGridPos = baseGridPos + glm::vec3(0.f, static_cast<float>(i), 0.f);
            createBlock(world, batchRenderer, trunkBlockGridPos * WORLD_SCALE, TRUNK_COLOR);
        }

        // Calculate the center of the leaves blob in grid coordinates.
        glm::vec3 leavesCenterGridPos = baseGridPos + glm::vec3(0.f, static_cast<float>(trunkHeight), 0.f);
        
        for (int x = -2; x <= 2; ++x) {
            for (int y = -1; y <= 2; ++y) {
                for (int z = -2; z <= 2; ++z) {
                    // Create a more organic, spherical shape
                    if (x * x + y * y + z * z > (2 * 2 + 1)) {
                        continue;
                    }
                    // Don't place leaves inside the top of the trunk
                    if (x == 0 && z == 0 && y <= 0) {
                        continue;
                    }

                    // The offset for each leaf block is in grid units.
                    glm::vec3 leafGridPos = leavesCenterGridPos + glm::vec3(x, y, z);
                    createBlock(world, batchRenderer, leafGridPos * WORLD_SCALE, LEAVES_COLOR);
                }
            }
        }
    }

    // Main world generation logic
    void generateWorld(flecs::world& world, flecs::entity batchRenderer) {
        // Initialize noise generator
        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetFrequency(TERRAIN_NOISE_FREQUENCY);
        
        // Seed the random number generator for tree placement ONCE.
        srand(time(NULL));

        for (int x = -WORLD_SIZE_X / 2; x < WORLD_SIZE_X / 2; ++x) {
            for (int z = -WORLD_SIZE_Z / 2; z < WORLD_SIZE_Z / 2; ++z) {
                // Get height from noise function. The output is [-1, 1].
                float noiseValue = noise.GetNoise(static_cast<float>(x), static_cast<float>(z));
                // Scale and offset to get a positive terrain height (in grid units).
                int height = static_cast<int>((noiseValue + 1.0f) / 2.0f * TERRAIN_HEIGHT_SCALE);

                // Create the top grass block
                // We calculate the position in "grid space" first.
                glm::vec3 grassGridPos(x, height, z);
                
                // We convert from grid space to world space by multiplying by WORLD_SCALE.
                createBlock(world, batchRenderer, grassGridPos * WORLD_SCALE, GRASS_COLOR);

                // Create dirt blocks underneath the grass
                for (int y = height - 1; y >= height - 4 && y >= 0; --y) {
                    // Also scale the position for dirt blocks.
                    createBlock(world, batchRenderer, glm::vec3(x, y, z) * WORLD_SCALE, DIRT_COLOR);
                }

                // Randomly decide to place a tree on top of the grass block
                float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Random float [0, 1]
                if (randomValue < TREE_DENSITY) {
                    // Calculate the tree's base position in grid space.
                    glm::vec3 treeBaseGridPos = grassGridPos + glm::vec3(0.f, 1.f, 0.f);
                    createTree(world, batchRenderer, treeBaseGridPos);
                }
            }
        }
    }
    
    void setupScene(flecs::world& world) {
         auto light = world.entity("Light");
        light.set<Rx::Component::DirectionalLight>({
            glm::vec3(1.f, 1.f, 1.f),  // Color
            10.f,                     // Intensity
        });
        light.set<Rx::Component::Transform>({
            glm::vec3(1.f,1.f,1.f), // Scale
            -3.14/4.f,                     // Angle
            glm::normalize(glm::vec3(1.f, 1.f, 0.f)), // Axis
            glm::vec3(0.f, 0.f, 50.f)  // Position
        });
    }
};




