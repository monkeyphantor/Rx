#pragma once
#include "application/application.hpp"
#include "component/flyingCamera.hpp"
#include "core/window.hpp"
namespace Rx {

	class RxGameWorld : public GameWorld
	{
	public:
		RxGameWorld(Application& app, flecs::world& world) : GameWorld(app, world) {
		}

		virtual ~RxGameWorld() = default;


		virtual void loadGlobal() override {
			GameWorld::loadGlobal();
			

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
}