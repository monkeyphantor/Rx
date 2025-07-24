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
#include "flyingCamera.hpp"
#include "input.hpp"
#include "time.hpp"
#include "velocity.hpp"
#include "Sensor.hpp"
#include "ContactInfo.hpp"
#include "application.hpp"
#define random(lower, upper) ((static_cast<float>(rand())/static_cast<float>(RAND_MAX))*((upper)-(lower)) + (lower))

struct Actors;
struct Fireball{};
struct Move;
struct LevelManager;

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


inline int fireballCounter{0};

struct CanShootFireball {};
struct FireballCooldown {
    float time;
};
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
        auto floor = world.entity("floor");
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

    //    for(int i = -5 ; i < 5; i++){
    //         for(int j = -5; j < 5; j++){

    //             auto block = world.entity();
    //             block.add(rel, batchRenderer);
    //             block.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
    //             block.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(i * 3.f, 50.f + std::abs(i*j), j * 3.f) });
    //             block.set<Rx::Component::Velocity>({.velocity = glm::vec3(0.f), .angularVelocity = glm::vec3(0.f)});
    //             block.set<Rx::Component::DynamicRigidBody>({ 
    //                 .objectLayer =  Rx::Layers::DYNAMIC_ENVIRONMENT, 
    //                 .mass = 1.0f,
    //                 .friction = 0.5f,
    //                 .restitution = 0.5f
    //             });
    //             block.set<Rx::Component::BoxCollider>({
    //                 .halfExtent = glm::vec3(1.f, 1.f, 1.f),
    //             });
    //         }
    //     }  

        world.lookup("FlyingCamera")
            .add<CanShootFireball>(); // Give the camera the ability to shoot fireballs

        world.system<FireballCooldown>()
        .each([](flecs::entity e, FireballCooldown& cd) {
            cd.time -= Rx::Time::deltaTime;
            if (cd.time <= 0) {
                e.remove<FireballCooldown>();
                e.add<CanShootFireball>();
            }
        });
      
         world.system<Rx::Component::FlyingCamera>()
            .with<CanShootFireball>()
			.kind(flecs::PreUpdate)
            .each([&](flecs::entity camEntity, Rx::Component::FlyingCamera& camera) {
            if(!Rx::Input::buttonLeft.down) {
                return; // Only create fireball if mouse button 1 is pressed
            }

            Rx::Component::Transform fireballTransform;
            fireballTransform.translation = camera.position;
            fireballTransform.scale = glm::vec3(1.f);
            fireballTransform.angle = 0.f;
            fireballTransform.axis = glm::vec3(1.f, 0.f, 0.f);

            float speed = 100.f;
            glm::vec3 velocity = camera.direction * speed;

            // Create a fireball entity
            auto fireball = camEntity.world().entity((std::string("fireball")+std::to_string(fireballCounter++)).c_str());
            auto rel = camEntity.world().lookup("InstancedColorMeshRelation");
            auto batchRenderer = camEntity.world().lookup("Actors::InstancedRender");
            fireball.add(rel, batchRenderer);
            fireball.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
            fireball.set<Rx::Component::Transform>(fireballTransform);
            fireball.set<Rx::Component::Velocity>({velocity, glm::vec3(0.f)});
            fireball.set<Rx::Component::KinematicSensor>({ 
                    .objectLayer =  Rx::Layers::PLAYER_SPELL, 
                });
            fireball.set<Rx::Component::BoxCollider>({
                .halfExtent = glm::vec3(1.f, 1.f, 1.f),
            });
            fireball.add<Fireball>();


			camEntity.remove<CanShootFireball>(); // Remove the ability to shoot again 
            camEntity.set<FireballCooldown>({ 0.5f }); // Set cooldown for 0.5 seconds
         });

          world.system<const Fireball>("FireballImpact")
             .kind(flecs::PostUpdate) // Run after all physics updates
              // This system captures the world and render entities to be able to spawn new objects
              .each([&](flecs::entity fireball, const Fireball&) {

                  // For each fireball, iterate its ContactInfo relationships to see what it hit
                  fireball.each<Rx::Component::ContactInfo>([&](flecs::entity target) {
                      const Rx::Component::ContactInfo& contact = fireball.get<Rx::Component::ContactInfo>(target);
                      // --- Pre-computation and Safety Checks ---

                      // If the fireball or target is already dead from a previous collision this frame, stop.
                      if (!fireball.is_alive() || !target.is_alive()) {
                          return;
                      }
                      

                      // --- Shatter Logic ---

                      const int shard_count = 5;
                      const float explosion_speed = 40.0f;
                      const float spread_factor = 0.8f; // How wide the explosion cone is

                      // Define world up vector
                      const glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
                    
                      // Calculate the basis vectors for our "upper half circle" fan.
                      // 'right' is a vector perpendicular to both the impact normal and the world up direction.
                      glm::vec3 right = glm::cross(world_up, contact.normal);
                      if (glm::length(right) < 0.001f) {
                          // Handle case where normal is parallel to world_up (e.g., hitting top of a block)
                          right = glm::vec3(1.0f, 0.0f, 0.0f);
                      }
                      right = glm::normalize(right);

                      // 'fan_up' is a vector perpendicular to the normal and our new 'right' vector.
                      // This gives us an "up" direction for the fan that is oriented with the surface.
                      glm::vec3 fan_up = glm::normalize(glm::cross(contact.normal, right));
                    
                      // Spawn the shards
                      for (int i = 0; i < shard_count; ++i) {
                          // Calculate the angle for this shard in the half-circle (0 to PI radians)
                          float angle = (static_cast<float>(i) / (shard_count - 1)) * 3.14159f;

                          // Create a direction vector in the fan plane
                          glm::vec3 spread_direction = (cos(angle) * right + sin(angle) * fan_up) * spread_factor;

                          // The final velocity is a combination of the outward normal and the spread direction.
                          // This creates a cone-like explosion pointing away from the surface.
                          glm::vec3 shard_velocity = glm::normalize(contact.normal + spread_direction) * explosion_speed;

                         auto rel = fireball.world().lookup("InstancedColorMeshRelation");
                         auto batchRenderer = fireball.world().lookup("Actors::InstancedRender");

                          // Create the shard entity
                          auto shard = world.entity();
                          shard.add(rel, batchRenderer);
                          shard.set<Rx::Component::Transform>({
                              glm::vec3(1.0f), // Scale
                              0.f, glm::vec3(0,1,0), // Rotation
                              contact.position-glm::normalize(shard_velocity)*4.f // Position: Spawn at the impact point
                          });
                          shard.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), 0.8f, 0.1f, glm::vec3(0.0f) });
                          shard.set<Rx::Component::DynamicRigidBody>({
                              .objectLayer = Rx::Layers::DYNAMIC_ENVIRONMENT,
                              .mass = 0.1f,
                              .friction = 0.5f,
                              .restitution = 0.4f
                          });
                          shard.set<Rx::Component::BoxCollider>({
                              .halfExtent = glm::vec3(0.2f)
                          });
                          // THIS IS THE KEY: Give the new shard its calculated velocity
                          shard.set<Rx::Component::Velocity>({ -shard_velocity });
                      }

                      // Destroy the original objects
                      fireball.destruct();

                  });

              });
    }
};




