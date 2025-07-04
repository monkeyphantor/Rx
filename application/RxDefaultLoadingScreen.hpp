#pragma once
#include "loadingScreen.hpp"
#include "flyingCamera.hpp"
#include "VkColorMesh.hpp"
#include "VkColorModelDescriptorSet.hpp"
struct Actor;
namespace Rx {

    class RxDefaultLoadingScreen : public Rx::LoadingScreen {
    public:
        // Constructor
        RxDefaultLoadingScreen(Application& app, flecs::world& world) : Rx::LoadingScreen(app, world) {
            // Initialize the loading screen
            std::cout << "RxLoadingScreen initialized." << std::endl;
        }

        // Destructor
        virtual ~RxDefaultLoadingScreen() = default;

        virtual void loadGlobal() override {
            LoadingScreen::loadGlobal();

            world.component<Rx::Component::FlyingCamera>("FlyingCamera").set<Rx::Component::FlyingCamera>({
            glm::vec3(0.f, 10.f, 10.f),
            Rx::Component::FlyingCamera::lookAt(glm::vec3(0.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f)),
            10.f,
            0.01f,
            Rx::Core::windowHeight / static_cast<float>(Rx::Core::windowWidth),
            0.1f,
            100000.f
            });

        world.system<Rx::Component::FlyingCamera>().kind(flecs::OnUpdate).each(
            [](flecs::entity e, Rx::Component::FlyingCamera& camera) {
                camera.update();
            });

            world.system()
            .kind(flecs::OnUpdate)
            .run([](flecs::iter& it) {
                // This is where you would update the loading screen.
                // For now, just print a message.
                std::cout << "[Loading Screen] Progressing loading..." << std::endl;
            });

         
            
            world.system<Rx::Component::Transform>()
            .with<Rx::Component::ColorMesh>()
            .with<Rx::Component::VkColorMesh>()
            .with<Rx::Component::VkColorModelDescriptorSet>()
            .kind(flecs::OnUpdate)
            .each([&](Rx::Component::Transform& transform) {
                // Rotate the cube around the Y-axis
                transform.angle += world.delta_time(); // Adjust speed as needed
                if (transform.angle > 2 * glm::pi<float>()) {
                    transform.angle -= 2 * glm::pi<float>();
                }
            });

            world.import<Actor>();
        }
    };

} // namespace Rx


struct Actor {
    Actor(flecs::world& world) {
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

        for (int i = 0; i < 1; i++) 
        {
            for (int j = 0; j < 1; j++)
            {
                auto e = world.entity();
                e.set<Rx::Component::ColorMesh>({ cubeVertices, cubeIndices });
                std::cout << "Loading Actor" << std::endl;
                e.add<Rx::Component::VkColorMesh>();
                e.add<Rx::Component::VkColorModelBuffer>();
                e.add<Rx::Component::VkColorModelDescriptorSet>();
                e.set<Rx::Component::Transform>({ glm::vec3(1.f), 0.f, glm::vec3(0.f, 1.f, 0.f), glm::vec3(i * 3.f, 0.f, j * 3.f) });
            }
        }
    }
};

