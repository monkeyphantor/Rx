#pragma once

#include "global.hpp"
#include "flecs.h"
#include "level.hpp"
#include "TextureMeshArray.hpp"
#include "VkTextureMeshArray.hpp"
#include "TextureArray.hpp"
#include "VkTextureArray.hpp"
#include "TextureMaterialBuffer.hpp"
#include "VkTextureMaterialBuffer.hpp"
#include "VkTransformBuffer.hpp"
#include "VkTextureModelDescriptorSet.hpp"
#include "shape.hpp"
#include "LoadTexture.hpp"
#include "IndirectBuffer.hpp"
#include "VkIndirectBuffer.hpp"
#include "transform.hpp"
#include "DirectionalLight.hpp"
#include "GameWorldBase.hpp"
#include "gameWorld.hpp"
#include "ColorMesh.hpp"
#include "VkColorMesh.hpp"
#include "VkColorModelBuffer.hpp"
#include "VkColorModelDescriptorSet.hpp"
#include "LoadSkeletonModel.hpp"
#include "flyingCamera.hpp"
#include "SkeletonBuffer.hpp"
#include "AnimationStateMachine.hpp"
#include "AnimationMap.hpp"
#include "Skeleton.hpp"
#include "AnimationClip.hpp"

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

        
        world.get_mut<Rx::Component::FlyingCamera>().speed = 100.f;

        auto asset = Rx::Asset::loadSkeletonModel("C:/Users/robry/Desktop/3DModels/wizard.skelmod.rx", world, "SkeletonModelAsset", 900);

        flecs::entity skeleton;
		asset.children(world.lookup("IsSkeletonOf"), [&](flecs::entity child) {
			skeleton = child;
		});
        auto animationPrefab = skeleton.get<Rx::Component::Skeleton>().animationPrefab;
        auto& map = animationPrefab.get<Rx::Component::AnimationMap>().animations;
        auto& animClip = animationPrefab.get<Rx::Component::AnimationClip>(map.at("Run"));
        Rx::Component::AnimationStateMachine animStateMachine;
        animStateMachine.addAnimationState("Run", Rx::Component::SingleAnimation{map.at("Run"), 0.0f, 1.f, animClip.duration, animClip.ticksPerSecond});
        animStateMachine.setCurrentState("Run");

        float space = 4.f;
        for (int i = 0; i < 30; i++)
        {
            for (int j = 0; j < 30; j++)
            {
                auto e = Rx::Asset::createSkeletonModelInstance(world, asset, "Wizard_" + std::to_string(i) + "_" + std::to_string(j));
                if(!e.is_valid()) {
                    RX_LOGE("Failed to create Wizard instance entity", std::to_string(i), std::to_string(j));
                }
                e.set<Rx::Component::Transform>({ glm::vec3(1.0f), 0.f * 3.14f/2.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(i * space, 0.f, j * space) });
                e.set<Rx::Component::AnimationStateMachine>(animStateMachine);
                e.add<LevelAsset>();
            }
        }
    }
};