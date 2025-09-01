#pragma once

#include "Global.hpp"
#include "Physics.hpp"
#include "flecs.h"
#include "Level.hpp"
#include "TextureMeshArray.hpp"
#include "VkTextureMeshArray.hpp"
#include "TextureArray.hpp"
#include "VkTextureArray.hpp"
#include "TextureMaterialBuffer.hpp"
#include "VkTextureMaterialBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureModelDescriptorSet.hpp"
#include "Shape.hpp"
#include "LoadTexture.hpp"
#include "IndirectBuffer.hpp"
#include "VkIndirectBuffer.hpp"
#include "Transform.hpp"
#include "DirectionalLight.hpp"
#include "GameWorldBase.hpp"
#include "GameWorld.hpp"
#include "ColorMesh.hpp"
#include "VkColorMesh.hpp"
#include "VkColorModelBuffer.hpp"
#include "VkColorModelDescriptorSet.hpp"
#include "LoadSkeletonModel.hpp"
#include "FlyingCamera.hpp"
#include "SkeletonBuffer.hpp"
#include "AnimationStateMachine.hpp"
#include "AnimationMap.hpp"
#include "Skeleton.hpp"
#include "AnimationClip.hpp"
#include "CharacterCapsule.hpp"
#include "CharacterController.hpp"
#include "CharacterCamera.hpp"  
#include "VkInstancedColorModelBuffer.hpp"
#include "VkInstancedColorModelDescriptorSet.hpp"
#include "RigidBody.hpp"
#include "BoxCollider.hpp"
#include "MeshTransform.hpp"
#include "Velocity.hpp"
#include "Sensor.hpp"
#include "ContactInfo.hpp"
#include "NpcController.hpp"
#include "FireballNpcLauncher.hpp"
#include "FollowPlayer.hpp"

#define random(lower, upper) ((static_cast<float>(rand())/static_cast<float>(RAND_MAX))*((upper)-(lower)) + (lower))

struct Actors;
class RxLevel : public Rx::Level
{
public:
     // Constructor
        RxLevel() = default;

        // Destructor
        virtual ~RxLevel() = default;

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
struct Player{};
struct Npc{};
struct Health{
    float value;
};
struct Hit{
  };

static int fireballCounter = 0;
struct Fireball{};
struct CanShootFireball{};
struct FireballCountdown{
    float countDown;
};
struct FireballCooldown {
    float time;
};

struct Actors{

    Actors(flecs::world& world){

        world.module<Actors>();

        auto light = world.entity("Light");
        light.set<Rx::Component::DirectionalLight>({
            glm::vec3(1.f, 1.f, 1.f),  // Color
            10.f,                     // Intensity
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

        world.system()
        .kind(flecs::PreUpdate)
        .run([batchRenderer](flecs::iter& it) {
                while(it.next()){
                    batchRenderer.add<Rx::ShouldBeUpdated>();
                }
            });
        // FLOOR
        auto rel = world.lookup("InstancedColorMeshRelation");
        auto floor = world.entity();
        floor.add(rel, batchRenderer);
        floor.set<Rx::Component::Transform>({
            glm::vec3(300.f,0.1f,300.f),// Scale
            0.f,                        // Angle
            glm::vec3(0.f, 1.f, 0.f),   // Axis
            glm::vec3(-0.f, 0.f, -0.f)  // Position
        });
        floor.set<Rx::Component::Material>({ glm::vec3(1.f, 0.1f, 0.5f), 0.9f, 0.9f, glm::vec3(0.0f) });
        floor.set<Rx::Component::StaticRigidBody>({ 
            .objectLayer =  Rx::Layers::STATIC_ENVIRONMENT, 
            .friction = 0.5f,
            .restitution = 0.5f});
        floor.set<Rx::Component::BoxCollider>({
            .halfExtent = glm::vec3(300.f, 0.1f, 300.f), // Large enough to cover the floor
        });

        // for(int i = -20 ; i < 20; i++){
        //     for(int j = -20; j < 20; j++){

        //         auto block = world.entity();
        //         block.add(rel, batchRenderer);
        //         block.set<Rx::Component::Material>({ glm::vec3(random(0.1f,0.9f), random(0.9f,1.f), random(0.1f,0.9f)), random(0.1f,0.1f), random(0.8f,0.9f), glm::vec3(0.0f) });
        //         block.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(random(-100.f, 100.f), random(50.f, 100.f), random(-100.f, 100.f)) });
        //         block.set<Rx::Component::Velocity>({
        //             .velocity = glm::vec3(random(-5.f, 5.f), random(-5.f, 5.f), random(-5.f, 5.f)),
        //             .angularVelocity = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f))
        //         });
        //         block.set<Rx::Component::DynamicRigidBody>({ 
        //             .objectLayer =  Rx::Layers::DYNAMIC_ENVIRONMENT, 
        //             .mass = 1000.0f,
        //             .friction = 1.0f,
        //             .restitution = 0.5f
        //         });
        //         block.set<Rx::Component::BoxCollider>({
        //             .halfExtent = glm::vec3(1.f, 1.f, 1.f),
        //         });
        //         block.add<Fireball>();
        //         block.set<Health>({ 100.f });
        //     }
        // }

        world.system<FireballCooldown>()
        .each([](flecs::entity e, FireballCooldown& cd) {
            cd.time -= Rx::Time::deltaTime;
            if (cd.time <= 0) {
                e.remove<FireballCooldown>();
                e.add<CanShootFireball>();
            }
        });

         world.system<Rx::Component::Transform, Rx::Component::Velocity, FireballNpcLauncher>()
            .with<CanShootFireball>()
            .with<Npc>()
			.kind(flecs::PreUpdate)
            .each([&](flecs::entity e, Rx::Component::Transform& tf, const Rx::Component::Velocity& vel, FireballNpcLauncher& launcher) {

            if(!launcher.shouldShoot) {
                return;
            }


            float countDown = 0.3f;
            Rx::Component::Transform fireballTransform;
            fireballTransform.translation = tf.translation + tf.forward() * (2 + glm::length(vel.velocity) * countDown * 2.f);
            fireballTransform.scale = glm::vec3(1.f);
            fireballTransform.angle = 0.f;
            fireballTransform.axis = glm::vec3(1.f, 0.f, 0.f);

            float speed = 30.f;
            glm::vec3 velocity = tf.forward() * speed;

            // Create a fireball entity
            auto fireball = e.world().entity((std::string("fireball")+std::to_string(fireballCounter++)).c_str());
            auto rel = e.world().lookup("InstancedColorMeshRelation");
            auto batchRenderer = e.world().lookup("Actors::InstancedRender");
            fireball.add(rel, batchRenderer);
            fireball.set<Rx::Component::Transform>(fireballTransform);
            fireball.set<Rx::Component::Velocity>({velocity, glm::vec3(0.f)});
            fireball.add<Fireball>();
            fireball.set<FireballCountdown>({ countDown });
            fireball.add<Npc>();
			e.remove<CanShootFireball>(); // Remove the ability to shoot again 
            e.set<FireballCooldown>({ 3.0f }); // Set cooldown for 1.0 seconds
            launcher.shouldShoot = false; // Reset fireball launcher
         });

        world.system<Rx::Component::Transform, Rx::Component::Velocity>()
            .with<CanShootFireball>()
            .with<Player>()
			.kind(flecs::PreUpdate)
            .each([&](flecs::entity e, Rx::Component::Transform& tf, const Rx::Component::Velocity& vel) {
            if(!Rx::Input::buttonLeft.down) {
                return; // Only create fireball if mouse button 1 is pressed
            }

            float countDown = 0.3f;
            Rx::Component::Transform fireballTransform;
            fireballTransform.translation = tf.translation + tf.forward() * (2 + glm::length(vel.velocity) * countDown * 2.f);
            fireballTransform.scale = glm::vec3(1.f);
            fireballTransform.angle = 0.f;
            fireballTransform.axis = glm::vec3(1.f, 0.f, 0.f);

            float speed = 60.f;
            glm::vec3 velocity = tf.forward() * speed;

            // Create a fireball entity
            auto fireball = e.world().entity((std::string("fireball")+std::to_string(fireballCounter++)).c_str());
            auto rel = e.world().lookup("InstancedColorMeshRelation");
            auto batchRenderer = e.world().lookup("Actors::InstancedRender");
            fireball.add(rel, batchRenderer);
            fireball.set<Rx::Component::Transform>(fireballTransform);
            fireball.set<Rx::Component::Velocity>({velocity, glm::vec3(0.f)});
            fireball.add<Fireball>();
            fireball.set<FireballCountdown>({ countDown });
            fireball.add<Player>();

			e.remove<CanShootFireball>(); // Remove the ability to shoot again 
            e.set<FireballCooldown>({ 0.5f }); // Set cooldown for 0.5 seconds
         });

        world.system<Fireball, FireballCountdown, Rx::Component::Transform, Rx::Component::Velocity>()
        .without<Rx::Component::KinematicSensor>()
        .with<Npc>()
        .kind(flecs::PreUpdate)
        .each([](flecs::entity fireball, const Fireball&, FireballCountdown& countdown, Rx::Component::Transform& tf, Rx::Component::Velocity& vel) {
            countdown.countDown -= Rx::Time::deltaTime;
            if (countdown.countDown <= 0) {
                auto rel = fireball.world().lookup("InstancedColorMeshRelation");
                auto batchRenderer = fireball.world().lookup("Actors::InstancedRender");
                fireball.add(rel, batchRenderer);
                fireball.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
                fireball.set<Rx::Component::KinematicSensor>({ 
                        .objectLayer =  Rx::Layers::ENEMY_SPELL, 
                    });
                fireball.set<Rx::Component::BoxCollider>({
                    .halfExtent = glm::vec3(1.f, 1.f, 1.f),
                });
            }
        });

        world.system<Fireball, FireballCountdown, Rx::Component::Transform, Rx::Component::Velocity>()
        .without<Rx::Component::KinematicSensor>()
        .with<Player>()
        .kind(flecs::PreUpdate)
        .each([](flecs::entity fireball, const Fireball&, FireballCountdown& countdown, Rx::Component::Transform& tf, Rx::Component::Velocity& vel) {
            countdown.countDown -= Rx::Time::deltaTime;
            if (countdown.countDown <= 0) {
                auto rel = fireball.world().lookup("InstancedColorMeshRelation");
                auto batchRenderer = fireball.world().lookup("Actors::InstancedRender");
                fireball.add(rel, batchRenderer);
                fireball.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), random(0.1f,0.2f), random(0.1f,0.2f), glm::vec3(0.0f) });
                fireball.set<Rx::Component::KinematicSensor>({ 
                        .objectLayer =  Rx::Layers::PLAYER_SPELL, 
                    });
                fireball.set<Rx::Component::BoxCollider>({
                    .halfExtent = glm::vec3(1.f, 1.f, 1.f),
                });
            }
        });
         world.system<const Fireball, Rx::Component::ContactInfo, Health>()
         .term_at(1).second("$parent")
         .term_at(2).src("$parent")
         .kind(flecs::PostUpdate)
         .run([&](flecs::iter& it) {
                      
            
                    while(it.next()){

                        flecs::entity target = it.id(1).second();
                        auto contactInfos =  it.field<Rx::Component::ContactInfo>(1);
                        for(auto i : it) {
                            // auto& contact = contactInfos[i];
                             auto fireball = it.entity(i);

                            // const int shard_count = 5;
                            // const float explosion_speed = 40.0f;
                            // const float spread_factor = 0.8f; // How wide the explosion cone is

                            // // Define world up vector
                            // const glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
                            
                            // // Calculate the basis vectors for our "upper half circle" fan.
                            // // 'right' is a vector perpendicular to both the impact normal and the world up direction.
                            // glm::vec3 right = glm::cross(world_up, contact.normal);
                            // if (glm::length(right) < 0.001f) {
                            //     // Handle case where normal is parallel to world_up (e.g., hitting top of a block)
                            //     right = glm::vec3(1.0f, 0.0f, 0.0f);
                            // }
                            // right = glm::normalize(right);

                            // // 'fan_up' is a vector perpendicular to the normal and our new 'right' vector.
                            // // This gives us an "up" direction for the fan that is oriented with the surface.
                            // glm::vec3 fan_up = glm::normalize(glm::cross(contact.normal, right));
                            
                            // // Spawn the shards
                            // for (int i = 0; i < shard_count; ++i) {
                            //     // Calculate the angle for this shard in the half-circle (0 to PI radians)
                            //     float angle = (static_cast<float>(i) / (shard_count - 1)) * 3.14159f;

                            //     // Create a direction vector in the fan plane
                            //     glm::vec3 spread_direction = (cos(angle) * right + sin(angle) * fan_up) * spread_factor;

                            //     // The final velocity is a combination of the outward normal and the spread direction.
                            //     // This creates a cone-like explosion pointing away from the surface.
                            //     glm::vec3 shard_velocity = glm::normalize(contact.normal + spread_direction) * explosion_speed;

                            //     auto rel = fireball.world().lookup("InstancedColorMeshRelation");
                            //     auto batchRenderer = fireball.world().lookup("Actors::InstancedRender");

                            //     // Create the shard entity
                            //     auto shard = world.entity();
                            //     shard.add(rel, batchRenderer);
                            //     shard.set<Rx::Component::Transform>({
                            //         glm::vec3(1.0f), // Scale
                            //         0.f, glm::vec3(0,1,0), // Rotation
                            //         contact.position-glm::normalize(shard_velocity)*4.f // Position: Spawn at the impact point
                            //     });
                            //     shard.set<Rx::Component::Material>({ glm::vec3(random(0.f,0.5f), random(0.9f,1.f), random(0.9f,1.f)), 0.8f, 0.1f, glm::vec3(0.0f) });
                            //     shard.set<Rx::Component::DynamicRigidBody>({
                            //         .objectLayer = Rx::Layers::DYNAMIC_ENVIRONMENT,
                            //         .mass = 0.1f,
                            //         .friction = 0.5f,
                            //         .restitution = 0.4f
                            //     });
                            //     shard.set<Rx::Component::BoxCollider>({
                            //         .halfExtent = glm::vec3(0.2f)
                            //     });
                            //     // THIS IS THE KEY: Give the new shard its calculated velocity
                            //     shard.set<Rx::Component::Velocity>({ -shard_velocity });

                            //}

                            fireball.add<Hit>();
                        }

                       target.add<Hit>();
                    }
                  });


        world.system<Hit, Health, Rx::Component::AnimationStateMachine>()
        .each([&](flecs::entity e, const Hit& hit, Health& health, Rx::Component::AnimationStateMachine& animState) {

            health.value -= 10;
            if(health.value <= 0.0f){
                animState.transitionToState("Death", 1.00f);
                e.remove<Rx::Component::CharacterCapsule>();
            }
            e.remove<Hit>();
        });

        world.system<Hit>()
        .without<Rx::Component::AnimationStateMachine>()
        .each([&](flecs::entity e, const Hit& hit) {
            e.destruct();
        });

        world.system<Rx::Component::CharacterCamera, Rx::Component::LocalTransform>().each([](Rx::Component::CharacterCamera& camera, Rx::Component::LocalTransform& localTransform) {
            camera.update(localTransform);
        });

        world.system()
        .run([&](flecs::iter& it){
            while(it.next()){
                auto wizard = it.world().lookup("Actors::Wizard");
                auto camera = it.world().lookup("Actors::WizardCamera");

                const auto& localTransform = camera.get<Rx::Component::LocalTransform>();
                auto& cameraTransform = camera.get_mut<Rx::Component::Transform>();

                const auto& wizardTransform = wizard.get<Rx::Component::Transform>();

                cameraTransform = Rx::Component::Transform::fromGlmMat4(wizardTransform.getTransformMatrix()*localTransform.getTransformMatrix());
            }
        });
        world.system<Rx::Component::CharacterCamera, Rx::Component::Transform>().each([](Rx::Component::CharacterCamera& camera, Rx::Component::Transform& transform) {
            camera.updateEye(transform);
        });

        auto asset = Rx::Asset::loadSkeletonModel("C:/Users/robry/Desktop/3DModels/wizard.skelmod.rx", world, "SkeletonModelAsset", 1600);

        auto character = Rx::Asset::createSkeletonModelInstance(world, asset, "Wizard");
        character.set<Rx::Component::Transform>({ glm::vec3(1.0f), 0.f * 3.14f/2.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 10.f, 0.f) });
        character.set<Rx::Component::MeshTransform>({ glm::vec3(1.0f), 3.14f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, -1.5f, 0.f) });
        character.add<Rx::Component::Velocity>();
        character.add<Rx::Component::AnimationStateMachine>();
        character.add<Rx::Component::CharacterController>();
        character.set<Rx::Component::CharacterCapsule>({Rx::Layers::PLAYER_BODY});
        character.add<LevelAsset>();
        character.add<CanShootFireball>();
        character.set<Health>({100.0f});
        character.add<Player>();
        auto camera = world.entity("WizardCamera");
        camera.set<Rx::Component::CharacterCamera>({ glm::vec2(0.f), 10.f, 0.01f, 3.14f/3.f, 0.1f, 1000.f });
        camera.add<Rx::Component::LocalTransform>();
        camera.add<Rx::Component::Transform>();

         world.observer<Rx::Component::AnimationStateMachine, Rx::Component::NpcController>()
        .event(flecs::OnAdd)
        .each([](flecs::entity e, Rx::Component::AnimationStateMachine& stateMachine, Rx::Component::NpcController& npcController) {
            npcController.initASM(e, stateMachine);
        });

        world.system<Rx::Component::NpcController, Rx::Component::CharacterCapsule, Rx::Component::AnimationStateMachine, Rx::Component::Transform, FireballNpcLauncher, FollowPlayer, Rx::Component::Transform>()
        .term_at(5).second("$parent")
        .term_at(6).src("$parent")
        .each([](Rx::Component::NpcController& npcController, Rx::Component::CharacterCapsule& capsule, Rx::Component::AnimationStateMachine& stateMachine, Rx::Component::Transform& transform, FireballNpcLauncher& fireballLauncher, const FollowPlayer& followPlayer, Rx::Component::Transform& playerTransform) {
            npcController.update(capsule, stateMachine, transform, playerTransform, fireballLauncher);
        });

        uint32_t numberNpcs = 100;
        for(uint32_t i = 0; i < numberNpcs; i++){
            auto npc = Rx::Asset::createSkeletonModelInstance(world, asset, "Npc_Wizard_" + std::to_string(i));
            npc.set<Rx::Component::Transform>({ glm::vec3(1.0f), 0.f * 3.14f/2.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(random(-150.f,-50.f), 2.f, random(-150.f,-50.f)) });
            npc.set<Rx::Component::MeshTransform>({ glm::vec3(1.0f), 3.14f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, -1.5f, 0.f) });
            npc.add<Rx::Component::Velocity>();
            npc.add<Rx::Component::NpcController>();
            npc.set<Rx::Component::CharacterCapsule>({Rx::Layers::ENEMY_BODY});
            npc.add<Rx::Component::AnimationStateMachine>();
            npc.add<FollowPlayer>(character);
            npc.set<Health>({9.0f});
            npc.add<Npc>();
            npc.set<FireballNpcLauncher>({false});
            npc.add<CanShootFireball>();
        }

       

    }
};