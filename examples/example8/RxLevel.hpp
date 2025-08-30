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

        for(int i = -20 ; i < 20; i++){
            for(int j = -20; j < 20; j++){

                auto block = world.entity();
                block.add(rel, batchRenderer);
                block.set<Rx::Component::Material>({ glm::vec3(random(0.1f,0.9f), random(0.9f,1.f), random(0.1f,0.9f)), random(0.1f,0.1f), random(0.8f,0.9f), glm::vec3(0.0f) });
                block.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(random(-100.f, 100.f), random(50.f, 100.f), random(-100.f, 100.f)) });
                block.set<Rx::Component::Velocity>({
                    .velocity = glm::vec3(random(-5.f, 5.f), random(-5.f, 5.f), random(-5.f, 5.f)),
                    .angularVelocity = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f))
                });
                block.set<Rx::Component::DynamicRigidBody>({ 
                    .objectLayer =  Rx::Layers::DYNAMIC_ENVIRONMENT, 
                    .mass = 1000.0f,
                    .friction = 1.0f,
                    .restitution = 0.5f
                });
                block.set<Rx::Component::BoxCollider>({
                    .halfExtent = glm::vec3(1.f, 1.f, 1.f),
                });
            }
        }

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
        character.add<Rx::Component::AnimationStateMachine>();
        character.add<Rx::Component::CharacterController>();
        character.set<Rx::Component::CharacterCapsule>({Rx::Layers::PLAYER_BODY});
        character.add<LevelAsset>();

        auto camera = world.entity("WizardCamera");
        camera.set<Rx::Component::CharacterCamera>({ glm::vec2(0.f), 10.f, 0.01f, 3.14f/3.f, 0.1f, 1000.f });
        camera.add<Rx::Component::LocalTransform>();
        camera.add<Rx::Component::Transform>();

    }
};