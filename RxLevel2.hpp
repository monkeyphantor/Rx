#pragma once
#include "./application/gameWorld.hpp"
#include "./application/level.hpp"
#include "./component/flyingCamera.hpp"
#include "./component/instancedColorGraphics.hpp"

struct Actors2;
struct FireballSystem2;
struct Move2;
namespace Rx {

    class RxLevel2 : public Rx::Level {
    public:
        // Constructor
        RxLevel2() = default;

        // Destructor
        virtual ~RxLevel2() = default;

        // Called when the level is loaded
        virtual void onLoad(flecs::world& world) override {
            // Implement level-specific loading logic here
                world.import<Actors2>();
                world.import<Move2>();
                world.import<FireballSystem2>();

        }

        // Called when the level is unloaded
        virtual void onUnload(flecs::world& world) override {
            // Implement level-specific unloading logic here
        }
    };

} // namespace Rx


 struct Actors2 {
     Actors2(flecs::world& world) {
         world.module<Actors2>();


         std::vector<Rx::Vertex::Color> cubeVertices(8);
         cubeVertices[0].position = glm::vec3(-1.f, -1.f, -1.f); // Front Bottom Left
         cubeVertices[0].color = glm::vec4(1.f, 0.f, 0.f, 0.1f);
         cubeVertices[1].position = glm::vec3(1.f, -1.f, -1.f);  // Front Bottom Right
         cubeVertices[1].color = glm::vec4(0.f, 1.f, 0.f, 0.1f);
         cubeVertices[2].position = glm::vec3(1.f, 1.f, -1.f);   // Front Top Right
         cubeVertices[2].color = glm::vec4(0.f, 0.f, 1.f, 0.1f);
         cubeVertices[3].position = glm::vec3(-1.f, 1.f, -1.f);  // Front Top Left
         cubeVertices[3].color = glm::vec4(1.f, 0.f, 1.f, 0.1f);
         cubeVertices[4].position = glm::vec3(-1.f, -1.f, 1.f);  // Back Bottom Left
         cubeVertices[4].color = glm::vec4(1.f, 1.f, 0.f, 0.1f);
         cubeVertices[5].position = glm::vec3(1.f, -1.f, 1.f);   // Back Bottom Right
         cubeVertices[5].color = glm::vec4(0.f, 1.f, 1.f, 0.1f);
         cubeVertices[6].position = glm::vec3(1.f, 1.f, 1.f);    // Back Top Right
         cubeVertices[6].color = glm::vec4(1.f, 1.f, 1.f, 0.1f);
         cubeVertices[7].position = glm::vec3(-1.f, 1.f, 1.f);   // Back Top Left
         cubeVertices[7].color = glm::vec4(0.f, 0.f, 0.f, 0.1f);

         std::vector<uint32_t> cubeIndices = {
             0, 1, 2, 2, 3, 0,    // Front face
             1, 5, 6, 6, 2, 1,    // Right face
             5, 4, 7, 7, 6, 5,    // Back face
             4, 0, 3, 3, 7, 4,    // Left face
             3, 2, 6, 6, 7, 3,    // Top face
             4, 5, 1, 1, 0, 4     // Bottom face
         };

        
         // --- Set up Batch Rendering Entity ---

         auto batchRenderEntity = world.entity("InstancedRender");
         batchRenderEntity.set<Rx::Component::ColorMesh>({ cubeVertices, cubeIndices });
         batchRenderEntity.add<Rx::Component::VkColorMesh>();
         Rx::Component::VkInstancedColorModelBuffer colorMeshInstanceBuffer;
         colorMeshInstanceBuffer.maxNumberInstances = 1000000;
         batchRenderEntity.set<Rx::Component::VkInstancedColorModelBuffer>(colorMeshInstanceBuffer);
         batchRenderEntity.add<Rx::Component::VkInstancedColorModelDescriptorSet>();

         auto rel = world.lookup("InstancedColorMeshRelation");

         for (int i = 0; i < 1000; i++) 
         {
             for (int j = 0; j < 1000; j++)
             {
                 auto e = world.entity();
                 e.add(rel, batchRenderEntity);
                 e.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(i * 2.f, 0.f, j * 2.f) });
                 e.add<LevelAsset>();
             }
         }
     }
 };

// void create_block(
//     flecs::world& world, 
//     const glm::vec3& grid_position, // Renamed to clarify it's a grid coordinate
//     flecs::entity renderer_entity, 
//     flecs::entity relation,
//     float world_scale) // NEW: The scale parameter
// {
//     auto e = world.entity();
//     e.add(relation, renderer_entity);

//     // MODIFIED: The Transform component is now calculated using world_scale.
//     // The scale of the cube mesh is set to half the world scale.
//     // The position is multiplied by the world scale to set the spacing.
//     //
//     // Why scale/2.0f? Your base cube mesh is 2 units wide (from -1.0 to +1.0).
//     // To make the final rendered cube have a width of 'world_scale', we must
//     // scale the base mesh by 'world_scale / 2.0f'.
//     e.set<Rx::Component::Transform>({ 
//         glm::vec3(world_scale / 2.0f),     // Scale of the block itself
//         0.f, 
//         glm::vec3(0.f, 1.f, 0.f), 
//         grid_position * world_scale        // Final world position (spacing)
//     });
//     e.add<LevelAsset>();
// }

// // MODIFIED: Helper function to build a tree.
// // Now accepts and passes down the 'world_scale'.
// void create_tree(
//     flecs::world& world, 
//     const glm::vec3& base_pos,
//     flecs::entity wood_renderer, flecs::entity wood_relation,
//     flecs::entity leaves_renderer, flecs::entity leaves_relation,
//     float world_scale) // NEW: The scale parameter
// {
//     // --- Create the Trunk ---
//     int trunk_height = 4 + (rand() % 3); // Random height between 4 and 6
//     for (int i = 1; i <= trunk_height; ++i) {
//         // MODIFIED: Pass the scale parameter
//         create_block(world, base_pos + glm::vec3(0, i, 0), wood_renderer, wood_relation, world_scale);
//     }

//     // --- Create the Leaves (Canopy) ---
//     glm::vec3 canopy_top = base_pos + glm::vec3(0, trunk_height, 0);
//     int canopy_radius = 2;

//     for (int y = -canopy_radius; y <= canopy_radius; ++y) {
//         for (int x = -canopy_radius; x <= canopy_radius; ++x) {
//             for (int z = -canopy_radius; z <= canopy_radius; ++z) {
//                 // Simple sphere-like shape for leaves
//                 if (x*x + y*y + z*z > (canopy_radius * canopy_radius - 1)) {
//                     continue;
//                 }
//                 // Don't place a leaf inside the top of the trunk
//                 if (x == 0 && z == 0 && y <= 0) {
//                     continue;
//                 }
                
//                 // MODIFIED: Pass the scale parameter
//                 create_block(world, canopy_top + glm::vec3(x, y, z), leaves_renderer, leaves_relation, world_scale);
//             }
//         }
//     }
// }

// flecs::entity create_block_renderer(flecs::world& world, const char* name, glm::vec4 color, uint32_t maxInstances = 1000000) {
//     // Basic cube geometry
//     std::vector<Rx::Vertex::Color> cubeVertices(8);
//     cubeVertices[0].position = glm::vec3(-1.f, -1.f, -1.f);
//     cubeVertices[1].position = glm::vec3(1.f, -1.f, -1.f);
//     cubeVertices[2].position = glm::vec3(1.f, 1.f, -1.f);
//     cubeVertices[3].position = glm::vec3(-1.f, 1.f, -1.f);
//     cubeVertices[4].position = glm::vec3(-1.f, -1.f, 1.f);
//     cubeVertices[5].position = glm::vec3(1.f, -1.f, 1.f);
//     cubeVertices[6].position = glm::vec3(1.f, 1.f, 1.f);
//     cubeVertices[7].position = glm::vec3(-1.f, 1.f, 1.f);
//     // Set the same color for all vertices of this block type
//     for (auto& vertex : cubeVertices) {
//         vertex.color = color;
//     }

//     std::vector<uint32_t> cubeIndices = {
//         0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 5, 4, 7, 7, 6, 5,
//         4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4
//     };

//     auto batchRenderEntity = world.entity(name);
//     batchRenderEntity.set<Rx::Component::Mesh>({ cubeVertices, cubeIndices });
//     batchRenderEntity.add<Rx::Component::ColorMesh>();
//     Rx::Component::VkInstancedColorModelBuffer colorMeshInstanceBuffer;
//     colorMeshInstanceBuffer.maxNumberInstances = maxInstances;
//     batchRenderEntity.set<Rx::Component::VkInstancedColorModelBuffer>(colorMeshInstanceBuffer);
//     batchRenderEntity.add<Rx::Component::VkInstancedColorModelDescriptorSet>();

//     return batchRenderEntity;

// }


// #define FNL_IMPL
// #include "FastNoiseLite.h"

// struct Actors2 {
//     Actors2(flecs::world& world) {
//         world.module<Actors2>();
        
//         // --- 1. Define Block Renderers ---
//         auto dirt_renderer = create_block_renderer(world, "DirtBlockRenderer", glm::vec4(0.3f, 0.5f, 0.2f, 1.0f), 800000);
//         auto wood_renderer = create_block_renderer(world, "WoodBlockRenderer", glm::vec4(0.4f, 0.2f, 0.05f, 1.0f), 100000);
//         auto leaves_renderer = create_block_renderer(world, "LeavesBlockRenderer", glm::vec4(0.1f, 0.8f, 0.2f, 1.0f), 400000);
//         auto rel = world.lookup("InstancedColorMeshRelation");

//         // --- 2. Setup Noise ---      
//         fnl_state noise = fnlCreateState();
//         noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
//         noise.frequency = 0.02f;

//         // --- 3. World Generation ---

//         // NEW: Define the master scale for the entire world.
//         // 2.0f mimics your original code's behavior (position * 2.0f).
//         // Try changing this to 1.0f for smaller blocks or 5.0f for huge blocks!
//         const float world_scale = 50.0f;

//         const int world_size = 300;
//         const float tree_chance = 0.015f;

//         std::cout << "Generating " << world_size << "x" << world_size << " world with scale " << world_scale << "..." << std::endl;

//         for (int x = 0; x < world_size; x++) 
//         {
//             for (int z = 0; z < world_size; z++)
//             {
//                 float noise_val = fnlGetNoise2D(&noise, (float)x, (float)z);
//                 int height = static_cast<int>(noise_val * 7.0f);

//                 // Create the top layer (grass block)
//                 glm::vec3 top_pos(x, height, z);
//                 // MODIFIED: Pass the scale
//                 create_block(world, top_pos, dirt_renderer, rel, world_scale);

//                 // Create the dirt blocks underneath
//                 for (int y = height - 1; y > height - 4; --y) {
//                     // MODIFIED: Pass the scale
//                     create_block(world, glm::vec3(x, y, z), dirt_renderer, rel, world_scale);
//                 }

//                 // --- Tree Spawning Logic ---
//                 float random_val = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//                 if (random_val < tree_chance) {
//                     // MODIFIED: Pass the scale
//                     create_tree(world, top_pos, wood_renderer, rel, leaves_renderer, rel, world_scale);
//                 }
//             }
//         }
//     }
// };

struct SinusRunning2{};
struct Move2 {
    Move2(flecs::world& world) {
        world.system<Rx::Component::Transform>().with<SinusRunning2>().src<SinusRunning2>().kind(flecs::OnUpdate)
            .run([](flecs::iter& it) {
            while (it.next()) {
                auto transform = it.field<Rx::Component::Transform>(0);
                for (auto i : it) {
                    transform[i].angle += 0.0001f;
                    transform[i].translation = glm::vec3(transform[i].translation[0],
                        20.f * glm::sin(transform[i].angle * 20 + transform[i].translation[2] / 4.f),
                        transform[i].translation[2]);
                }
            }
                });

        world.system<>().kind(flecs::OnUpdate).run([](flecs::iter& it) {
            // Update the loading screen UI here.
            if(Rx::Input::keyX.pressed) {
                it.world().defer_begin();
                if (it.world().has<SinusRunning2>()) {
                    it.world().remove<SinusRunning2>();
                } else {
                    it.world().add<SinusRunning2>();
                }
                it.world().defer_end();
            }
        });

    }


};

// New Fireball component
struct Fireball2 {
    glm::vec3 velocity;
};

// Fireball system: update position of fireballs each frame.
struct FireballSystem2 {
    FireballSystem2(flecs::world& world) {
        world.component<Fireball2>();
        world.system<Fireball2, Rx::Component::Transform>().kind(flecs::OnUpdate)
            .each([](flecs::entity e, Fireball2& fireball, Rx::Component::Transform& transform) {
                    transform.translation += fireball.velocity * Rx::Time::deltaTime * 0.1f;
                });
    


        world.system<Rx::Component::FlyingCamera>()
            .each([&](flecs::entity camEntity, Rx::Component::FlyingCamera& camera) {
            if(!Rx::Input::buttonLeft.pressed) {
                return; // Only create fireball if mouse button 1 is pressed
            }
            // Create a fireball entity
            auto fireball = world.entity();

            std::vector<Rx::Vertex::Color> cubeVertices(8);
            cubeVertices[0].position = glm::vec3(-1.f, -1.f, -1.f); // Front Bottom Left
            cubeVertices[0].color = glm::vec4(1.f, 0.f, 0.f, 0.1f);
            cubeVertices[1].position = glm::vec3(1.f, -1.f, -1.f);  // Front Bottom Right
            cubeVertices[1].color = glm::vec4(0.f, 1.f, 0.f, 0.1f);
            cubeVertices[2].position = glm::vec3(1.f, 1.f, -1.f);   // Front Top Right
            cubeVertices[2].color = glm::vec4(0.f, 0.f, 1.f, 0.1f);
            cubeVertices[3].position = glm::vec3(-1.f, 1.f, -1.f);  // Front Top Left
            cubeVertices[3].color = glm::vec4(1.f, 0.f, 1.f, 0.1f);
            cubeVertices[4].position = glm::vec3(-1.f, -1.f, 1.f);  // Back Bottom Left
            cubeVertices[4].color = glm::vec4(1.f, 1.f, 0.f, 0.1f);
            cubeVertices[5].position = glm::vec3(1.f, -1.f, 1.f);   // Back Bottom Right
            cubeVertices[5].color = glm::vec4(0.f, 1.f, 1.f, 0.1f);
            cubeVertices[6].position = glm::vec3(1.f, 1.f, 1.f);    // Back Top Right
            cubeVertices[6].color = glm::vec4(1.f, 1.f, 1.f, 0.1f);
            cubeVertices[7].position = glm::vec3(-1.f, 1.f, 1.f);   // Back Top Left
            cubeVertices[7].color = glm::vec4(0.f, 0.f, 0.f, 0.1f);

            std::vector<uint32_t> cubeIndices = {
                0, 1, 2, 2, 3, 0,    // Front face
                1, 5, 6, 6, 2, 1,    // Right face
                5, 4, 7, 7, 6, 5,    // Back face
                4, 0, 3, 3, 7, 4,    // Left face
                3, 2, 6, 6, 7, 3,    // Top face
                4, 5, 1, 1, 0, 4     // Bottom face
            };

            Rx::Component::Transform fireballTransform;
            fireballTransform.translation = camera.position;
            fireballTransform.scale = glm::vec3(1.f);
            fireballTransform.angle = 0.f;
            fireballTransform.axis = glm::vec3(1.f, 0.f, 0.f);

            float speed = 100.f;
            glm::vec3 velocity = camera.direction * speed;

            
            fireball.set<Rx::Component::ColorMesh>({ cubeVertices, cubeIndices });
            fireball.add<Rx::Component::VkColorMesh>();
            fireball.add<Rx::Component::VkColorModelDescriptorSet>();
            fireball.set<Rx::Component::Transform>(fireballTransform);
            fireball.set<Fireball2>({ velocity });


            });

        }
};




