#pragma once

#include "PhysicsGameWorld.hpp"
#include "window.hpp"
#include "flyingCamera.hpp"


    class GameWorldExample3 : public Rx::PhysicsGameWorld {
    public:
        GameWorldExample3(Rx::Application& app, flecs::world& world) : Rx::PhysicsGameWorld(app, world) {
        }

        void loadGlobal() override {
            Rx::PhysicsGameWorld::loadGlobal();
            // Register components, systems, and observers specific to this game world
            world.component<Rx::Component::FlyingCamera>("FlyingCamera").set<Rx::Component::FlyingCamera>({
                glm::vec3(0.f, 10.f, 10.f),
                Rx::Component::FlyingCamera::lookAt(glm::vec3(0.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f)),
                800.f,
                0.01f,
                Rx::Core::windowHeight / static_cast<float>(Rx::Core::windowWidth),
                0.1f,
                100000.f
                });

            world.system<Rx::Component::FlyingCamera>().kind(flecs::OnUpdate).each(
                [](flecs::entity e, Rx::Component::FlyingCamera& camera) {
                    camera.update();
                });         
        }
    };