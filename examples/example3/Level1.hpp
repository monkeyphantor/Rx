#pragma once
#include "gameWorld.hpp"
#include "level.hpp"
#include "DirectionalLight.hpp"
#include "VkColorModelBuffer.hpp"
#include "ColorMesh.hpp"
#include "Material.hpp"
#include "shape.hpp"
#include "BoxCollider.hpp"
#include "PhysicsBody.hpp"
#include "RigidBody.hpp"
#include "Physics.hpp"

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
 Actors(flecs::world& world) {
        world.module<Actors>();

        auto light = world.entity("Light");
        light.set<Rx::Component::DirectionalLight>({
            glm::vec3(1.f, 1.f, 1.f),  // Color
            100.f,                     // Intensity
        });
        light.set<Rx::Component::Transform>({
            glm::vec3(1.f,1.f,1.f), // Scale
            -3.14/4.f,                     // Angle
            glm::normalize(glm::vec3(1.f, 1.f, 0.f)), // Axis
            glm::vec3(0.f, 0.f, 0.f)  //
        });

        Rx::Shape::ColorCube cube(glm::vec3(1.f, 1.f, 1.f), glm::vec4(0.f, 1.f, 0.f,1.f));


        auto batchRenderer = world.entity("InstancedRender");
        batchRenderer.set<Rx::Component::ColorMesh>({ cube.getVertices(), cube.getIndices() });
        batchRenderer.add<Rx::Component::VkColorMesh>();

        Rx::Component::VkInstancedColorModelBuffer colorMeshInstanceBuffer;
        colorMeshInstanceBuffer.maxNumberInstances = 10000;
        batchRenderer.set<Rx::Component::VkInstancedColorModelBuffer>(colorMeshInstanceBuffer);
        batchRenderer.add<Rx::Component::VkInstancedColorModelDescriptorSet>();
        batchRenderer.add<Rx::ShouldBeUpdated>();


        // FLOOR
        auto rel = world.lookup("InstancedColorMeshRelation");
        auto floor = world.entity();
        floor.add(rel, batchRenderer);
        floor.set<Rx::Component::Transform>({
            glm::vec3(300.f,0.1f,300.f),// Scale
            0.f,                        // Angle
            glm::vec3(0.f, 1.f, 0.f),   // Axis
            glm::vec3(0.f)              // Position
        });
        floor.set<Rx::Component::Material>({ glm::vec3(1.f, 0.1f, 0.5f), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
        floor.set<Rx::Component::StaticRigidBody>({ 
            .objectLayer =  Rx::Layers::STATIC_ENVIRONMENT, 
            .friction = 0.5f,
            .restitution = 0.5f});
        floor.set<Rx::Component::BoxCollider>({
            .halfExtent = glm::vec3(300.f, 0.1f, 300.f), // Large enough to cover the floor
        });

        world.system()
            .kind(flecs::PreUpdate)
            .run([batchRenderer](flecs::iter& it) {
                  while(it.next()){
                        batchRenderer.add<Rx::ShouldBeUpdated>();
                  }
                });

       for(int i = -30 ; i < 30; i++){
            for(int j = -30; j < 30; j++){

                auto block = world.entity();
                block.add(rel, batchRenderer);
                // Set the color via the material. Roughness/metallic can be constant or varied.
                block.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
                block.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(i * 3.f, 50.f + std::abs(i*j), j * 3.f) });
                block.set<Rx::Component::DynamicRigidBody>({ 
                    .objectLayer =  Rx::Layers::DYNAMIC_ENVIRONMENT, 
                    .mass = 1.0f,
                    .friction = 0.5f,
                    .restitution = 0.5f
                });
                block.set<Rx::Component::BoxCollider>({
                    .halfExtent = glm::vec3(1.f, 1.f, 1.f),
                });
            }
        }   
    }
};




