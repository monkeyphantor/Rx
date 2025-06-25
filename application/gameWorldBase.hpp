 #pragma once

 #include "../core/command.hpp"
 #include "../core/renderpass.hpp"
 #include "../core/mutex.hpp"
 #include "../flecs/include/flecs.h"
#include "level.hpp"
#include "../component/colorGraphics.hpp"
#include "../component/colorMesh.hpp"
#include "../component/transform.hpp"




namespace Rx{

    struct RenderRunning{};
    struct ShouldBeRendered{};

    class Application;
    class GameWorldBase {
    public:
        GameWorldBase(Application& app, flecs::world& world) : world(world), app(app) {
        }

        flecs::world& getWorld() {
            return world;
        }

    public:
        virtual void loadGlobal() {
            world.set_ctx(this);

            registerComponentsBase();

            game = world.entity("Game");
            game.add<RenderRunning>();

            registerGraphicsBase();

        };


        
    protected:
        Application& app;

        flecs::world& world;
        flecs::entity game;

        //render entities
        flecs::entity onRenderBegin;
        flecs::entity onRender;
        flecs::entity onRenderEnd;


    private:
        void registerComponentsBase() {
            world.component<RenderRunning>();
            world.component<Rx::Component::Transform>();
        }

        void registerGraphicsBase(){

            world.component<Rx::Component::ColorGraphics>();
            world.component<Rx::Component::Mesh>();
            world.component<Rx::Component::ColorMesh>();
            world.component<Rx::Component::Transform>();            

            world.observer<Rx::Component::ColorGraphics>()
            .event(flecs::OnAdd)
            .each(graphics_component_on_add);
            world.observer<Rx::Component::ColorGraphics>()
            .event(flecs::OnRemove)
            .each(graphics_component_on_remove);

            world.observer<Rx::Component::Mesh, Rx::Component::ColorMesh>()
            .event(flecs::OnAdd)
            .each(mesh_component_on_add);

            world.observer<Rx::Component::ColorMesh>()
            .event(flecs::OnRemove)
            .each(mesh_component_on_remove);
       
            // Set up render entities
            onRenderBegin = world.entity("OnRenderBegin");
            onRenderBegin.depends_on(flecs::OnUpdate);
            onRender = world.entity("OnRender");
            onRender.depends_on(onRenderBegin);
            onRenderEnd = world.entity("OnRenderEnd");
            onRenderEnd.depends_on(onRender);


            world.system()
            .kind(onRenderBegin)
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {
                 while(it.next()) {
                     Rx::Core::vulkanCommandMutex.lock();
                     Rx::Core::beginCommand(Rx::Core::command[Rx::Core::commandIndex]);
                     Rx::Core::beginRenderPass(Rx::Core::command[Rx::Core::commandIndex]);
                 }
            });

            world.system<Rx::Component::ColorGraphics, Rx::Component::ColorMesh, Rx::Component::Transform>()
            .kind(onRender)
           // .with<ShouldBeRendered>()
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {    
                std::cout << "iterate render system" << std::endl;      
            
                while (it.next()) {
                auto graphics = it.field<Rx::Component::ColorGraphics>(0);
                auto meshes = it.field<Rx::Component::ColorMesh>(1);
                auto transforms = it.field<Rx::Component::Transform>(2);

                    for (auto i : it)
                    {

                        graphics[i].setModelBuffer(transforms[i].getTransformMatrix());

                        vkCmdBindPipeline
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics[i].getGraphicsPipeline());
                        
                        
                        VkDeviceSize offset[] = { 0 };    
                        vkCmdBindVertexBuffers
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        0, 1, 
                        &meshes[i].getVertexBuffer(), offset);
                    
                        
                        vkCmdBindIndexBuffer
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        meshes[i].getIndexBuffer(),
                        0, VK_INDEX_TYPE_UINT32);
                    
                        vkCmdBindDescriptorSets
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        graphics[i].getPipelineLayout(),
                        0, 1,
                        &graphics[i].getDescriptorSet(),
                        0, nullptr);
                    
                        vkCmdDrawIndexed
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        meshes[i].getNumberIndices(), 
                        1, 0, 0, 0);
                    }

                }
            
            });
            

            world.system()
            .kind(onRenderEnd)
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {
                while(it.next()) {
                    Rx::Core::endRenderPass(Rx::Core::command[Rx::Core::commandIndex]);
                    Rx::Core::endCommand(Rx::Core::command[Rx::Core::commandIndex]);
                    Rx::Core::submitGraphicsCommand(Rx::Core::command[Rx::Core::commandIndex]);
                    Rx::Core::presentGraphics(Rx::Core::command[Rx::Core::commandIndex]);
                    Rx::Core::vulkanCommandMutex.unlock();
                }
            });
            
        }

    };
}