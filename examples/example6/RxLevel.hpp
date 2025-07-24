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
#include "LoadTextureModel.hpp"
#include "flyingCamera.hpp"

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

        Rx::Shape::ColorCube cube(glm::vec3(100.f, 1.f, 100.f), glm::vec4(0.f, 1.f, 0.f,1.f));

        //Rx::Shape::ColorSphere sphere(1.f, 16, 16, glm::vec3(1.f, 0.f, 0.f));


        // auto e = world.entity();
        // e.set<Rx::Component::ColorMesh>({ cube.getVertices(), cube.getIndices() });
        // e.add<Rx::Component::VkColorMesh>();
        // e.add<Rx::Component::VkColorModelBuffer>();
        // e.add<Rx::Component::VkColorModelDescriptorSet>();
        // e.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, -2.f, 0.f) });
        // e.set<Rx::Component::Material>({ glm::vec4(1.f, 0.f, 0.f, 1.f), 0.5f, 0.5f, glm::vec3(0.f) });
   
        world.get_mut<Rx::Component::FlyingCamera>().speed = 100.f;

        auto asset = Rx::Asset::loadTextureModel("C:/Users/robry/Desktop/3DModels/sponza_palace.texmod.rx", world, "TextureModelAsset", 1024);
        auto rel = world.lookup("TextureModelInstanceRelation");


        float space = 40.f;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                auto e = world.entity();
                e.add(rel, asset);
                e.set<Rx::Component::Transform>({ glm::vec3(0.01f), 3.14f/2.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(i * space, 0.f, j * space) });
                e.add<LevelAsset>();
            }
        }
    }
};