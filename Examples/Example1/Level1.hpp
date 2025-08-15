#pragma once
#include "GameWorld.hpp"
#include "Level.hpp"
#include "DirectionalLight.hpp"
#include "VkColorModelBuffer.hpp"
#include "ColorMesh.hpp"
#include "VkColorMesh.hpp"
#include "VkColorModelDescriptorSet.hpp"
#include "Material.hpp"
#include "Shape.hpp"

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

        //Rx::Shape::ColorSphere sphere(1.f, 16, 16, glm::vec3(1.f, 0.f, 0.f));


        for(int i = 0 ; i < 10; i++){
            for(int j = 0; j < 10; j++){
                auto e = world.entity();
                e.set<Rx::Component::ColorMesh>({ cube.getVertices(), cube.getIndices() });
                e.add<Rx::Component::VkColorMesh>();
                e.add<Rx::Component::VkColorModelBuffer>();
                e.add<Rx::Component::VkColorModelDescriptorSet>();
                e.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(10.f,0.f,10.f) + glm::vec3(i * 7.f, 0.f, j * 7.f) });
                e.set<Rx::Component::Material>({ glm::vec4(1.f, 0.f, 0.f, 1.f), i/10.f, j/10.f, glm::vec3(0.f) });
            }
        }   
    }
};




