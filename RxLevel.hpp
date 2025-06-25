#pragma once
#include "./application/gameWorld.hpp"
#include "./application/level.hpp"
#include "RxLevel2.hpp"
struct Actors;
struct FireballSystem;
struct Move;
struct LevelManager;
namespace Rx {

    class RxLevel : public Rx::Level {
    public:
        // Constructor
        RxLevel() = default;

        // Destructor
        virtual ~RxLevel() = default;

        // Called when the level is loaded
        virtual void onLoad(flecs::world& world) override {
            // Implement level-specific loading logic here
                world.import<Actors>();
                world.import<Move>();
                world.import<FireballSystem>();
                world.import<LevelManager>();
        }

        // Called when the level is unloaded
        virtual void onUnload(flecs::world& world) override {
            // Implement level-specific unloading logic here
        }
    };

} // namespace Rx

struct LevelManager {
    LevelManager(flecs::world& world) {
        world.module<LevelManager>();
        
        world.system<>().kind(flecs::OnLoad).run([&](flecs::iter& it) {
        
            while(it.next()) {
                auto player = it.world().entity("FlyingCamera");
                if (!player.has<Rx::Component::FlyingCamera>()) {
                    RX_LOGE("RxLevel","LevelManager","Player entity does not have a FlyingCamera component.");
                }else{
                    auto cam = player.get<Rx::Component::FlyingCamera>();
                        if(glm::length(cam.position) > 1000.f){
                            
                            static_cast<Rx::GameWorld*>(world.get_ctx())->requestLevelSwitch<Rx::RxLevel2>();
                            RX_LOGI("RxLevel","LevelManager","Switching to RxLevel2 due to player position.");
                        }
                }
            
            
            }

        }).add<LevelAsset>();
    }
};
struct Actors {
    Actors(flecs::world& world) {
        world.module<Actors>();


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

        for (int i = 0; i < 0; i++) 
        {
            for (int j = 0; j < 30; j++)
            {
                auto e = world.entity();
                e.set<Rx::Component::Mesh>({ cubeVertices, cubeIndices });
				auto size = e.get<Rx::Component::Mesh>().vertices.size();    
                std::cout << "Loading Actors: size:" << size << std::endl;
                e.add<Rx::Component::ColorMesh>();
                e.add<Rx::Component::ColorGraphics>();
                e.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(i * 3.f, 0.f, j * 3.f) });
                e.add<LevelAsset>();
            }
        }
    }
};

struct SinusRunning{};
struct Move {
    Move(flecs::world& world) {
        world.system<Rx::Component::Transform>().with<SinusRunning>().src<SinusRunning>().kind(flecs::OnUpdate)
            .run([](flecs::iter& it) {
            while (it.next()) {
                auto transform = it.field<Rx::Component::Transform>(0);
                for (auto i : it) {
                    transform[i].angle += 0.01f;
                    transform[i].translation = glm::vec3(transform[i].translation[0],
                        20.f * glm::sin(transform[i].angle * 20 + transform[i].translation[2] / 4.f),
                        transform[i].translation[2]);
                }
            }
                }).add<LevelAsset>();

        world.system<>().kind(flecs::OnUpdate).run([](flecs::iter& it) {
            // Update the loading screen UI here.
            if(Rx::Input::keyX.pressed) {
                it.world().defer_begin();
                if (it.world().has<SinusRunning>()) {
                    it.world().remove<SinusRunning>();
                } else {
                    it.world().add<SinusRunning>();
                }
                it.world().defer_end();
            }
        }).add<LevelAsset>();

    }


};

// New Fireball component
struct Fireball {
    glm::vec3 velocity;
};

// Fireball system: update position of fireballs each frame.
struct FireballSystem {
    FireballSystem(flecs::world& world) {
        world.system<Fireball, Rx::Component::Transform>().kind(flecs::OnUpdate)
            .each([](flecs::entity e, Fireball& fireball, Rx::Component::Transform& transform) {
                    transform.translation += fireball.velocity * Rx::Time::deltaTime;
                }).add<LevelAsset>();
    


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

            
            fireball.set<Rx::Component::Mesh>({ cubeVertices, cubeIndices });
            fireball.add<Rx::Component::ColorMesh>();
            fireball.add<Rx::Component::ColorGraphics>();
            fireball.set<Rx::Component::Transform>(fireballTransform);
            fireball.set<Fireball>({ velocity });
            fireball.add<LevelAsset>();
            
            }).add<LevelAsset>();

        }
};




