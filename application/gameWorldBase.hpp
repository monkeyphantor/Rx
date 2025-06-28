 #pragma once

 #include "../core/command.hpp"
 #include "../core/renderpass.hpp"
 #include "../core/mutex.hpp"
 #include "../flecs/include/flecs.h"
#include "level.hpp"
#include "../component/colorGraphics.hpp"
#include "../component/colorMesh.hpp"
#include "../component/transform.hpp"
#include "../component/colorMeshInstanceBuffer.hpp"
#include "../component/colorMeshArray.hpp"
#include "../component/colorArrayGraphics.hpp"
#include "../component/indirectBuffer.hpp"
#include "../component/colorMeshInstance.hpp"
#include "../core/pipelineLayouts.hpp"
#include "../core/pipelines.hpp"

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
            registerObserversBase();

            game = world.entity("Game");
            game.add<RenderRunning>();

            registerGraphicsBase();

        };


        
    protected:
        Application& app;

        flecs::world& world;
        flecs::entity game;

        //render entities
        flecs::entity colorMeshArrayInstanceRelation;
        flecs::entity postUpdate;
        flecs::entity preRender;
        flecs::entity onRenderBegin;
        flecs::entity onRender;
        flecs::entity onRenderEnd;


    private:
        void registerComponentsBase() {
            world.component<RenderRunning>();
            world.component<Rx::Component::Transform>();

            world.component<Rx::Component::ColorGraphics>();
            world.component<Rx::Component::Mesh>();
            world.component<Rx::Component::ColorMesh>();
            world.component<Rx::Component::Transform>();            

            world.component<Rx::Component::MeshArray>();
            world.component<Rx::Component::ColorMeshArray>();
            world.component<Rx::Component::ColorMeshInstanceBuffer>();
            world.component<Rx::Component::IndirectBuffer>();
            world.component<Rx::Component::ColorArrayGraphics>();
            world.component<VkDrawIndexedIndirectCommand>();
        }

        void registerObserversBase(){
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

            world.observer<Rx::Component::MeshArray, Rx::Component::ColorMeshArray>()
            .event(flecs::OnAdd)
            .each(Rx::Component::colorMeshArray_component_on_add);

            world.observer<Rx::Component::ColorMeshArray>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorMeshArray_component_on_remove);

            world.observer<Rx::Component::ColorMeshInstanceBuffer>()
            .event(flecs::OnSet)
            .each(Rx::Component::colorMeshInstanceBuffer_on_set);

            world.observer<Rx::Component::ColorMeshInstanceBuffer>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorMeshInstanceBuffer_on_remove);

            world.observer<Rx::Component::IndirectBuffer>()
            .event(flecs::OnSet)
            .each(Rx::Component::indirectBuffer_component_on_set);

            world.observer<Rx::Component::IndirectBuffer>()
            .event(flecs::OnRemove)
            .each(Rx::Component::indirectBuffer_component_on_remove);

            world.observer<Rx::Component::ColorMeshInstanceBuffer, Rx::Component::ColorArrayGraphics>()
            .event(flecs::OnAdd)
            .each(Rx::Component::colorArrayGraphics_component_on_add);

            world.observer<Rx::Component::ColorArrayGraphics>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorArrayGraphics_component_on_remove);

        }

        void registerGraphicsBase(){

            
            // Set up render entities
            colorMeshArrayInstanceRelation = world.entity("ColorMeshArrayInstanceRelation");
            postUpdate = world.entity("PostUpdate");
            postUpdate.depends_on(flecs::OnUpdate);
            preRender = world.entity("PreRender");
            preRender.depends_on(postUpdate);
            onRenderBegin = world.entity("OnRenderBegin");
            onRenderBegin.depends_on(preRender);
            onRender = world.entity("OnRender");
            onRender.depends_on(onRenderBegin);
            onRenderEnd = world.entity("OnRenderEnd");
            onRenderEnd.depends_on(onRender);

            world.system("IndirectBufferReset")
            .with<Rx::Component::IndirectBuffer>()
            .kind(postUpdate)
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) { // Use .run() instead of .each()

                while(it.next()) {
                    // This system iterates over all entities with the IndirectBuffer component.
                    auto indirectBuffers = it.field<Rx::Component::IndirectBuffer>(0);
                    for (auto i : it) {
                        indirectBuffers[i].numberCommands = 0; // Reset the command count for the indirect buffer.
                    }
                }
            });
                
            world.system("ColorMeshArrayUpdate")
                .with<Rx::Component::Transform>()
                .with<VkDrawIndexedIndirectCommand>() // This is the command type we are interested in.
                // Find entities that have a (MeshArrayInstance, *) relationship.
                .with(colorMeshArrayInstanceRelation, flecs::Wildcard) // Group by the target of the MeshArrayInstance relationship.
                .group_by(colorMeshArrayInstanceRelation)
                .kind(preRender)
                .run([](flecs::iter& it) {
                    // This part is EXACTLY the same as before.
                    // The logic is decoupled from the specific relationship type.
                    while(it.next()) {
                        flecs::entity parent = it.world().entity(it.group_id());

                        RX_ASSERT(parent.is_alive(), "ColorMeshArrayUpdate", "System", "Parent entity is not alive");
                        RX_ASSERT(parent.has<Rx::Component::IndirectBuffer>(), "ColorMeshArrayUpdate", "System", "Parent entity does not have IndirectBuffer component");
                        RX_ASSERT(parent.has<Rx::Component::ColorMeshInstanceBuffer>(), "ColorMeshArrayUpdate", "System", "Parent entity does not have ColorMeshInstanceBuffer component");

                        auto& buffer_comp = parent.get_mut<Rx::Component::IndirectBuffer>();
                        auto& instance_comp = parent.get_mut<Rx::Component::ColorMeshInstanceBuffer>();

                        VkDrawIndexedIndirectCommand* commands_dst = (VkDrawIndexedIndirectCommand*) buffer_comp.buffer.pMemory;
                        size_t buffer_capacity = buffer_comp.maxNumberCommands;
                        uint32_t draw_index = 0;

                        Rx::Component::ColorMeshInstance* instances = (Rx::Component::ColorMeshInstance*) instance_comp.instanceBuffer.pMemory;
                        size_t instance_capacity = instance_comp.maxNumberInstances;

                        const auto& src_instances = it.field<const Rx::Component::Transform>(0);
                        const auto& src_command = it.field<const VkDrawIndexedIndirectCommand>(1);

                        for (auto i : it) {
                            if (draw_index >= buffer_capacity || draw_index >= instance_capacity) {
                                std::cerr << "Error: buffer overflow for parent " 
                                        << parent.name() << "!\n";
                                break;
                            }
                            
                            commands_dst[draw_index] = src_command[i];
                            instances[draw_index].transform = src_instances[i].getTransformMatrix();

                            draw_index++;
                        }
                        buffer_comp.numberCommands = draw_index;
                    }
                });

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
            
                vkCmdBindPipeline
                (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                Rx::Core::colorMeshPipeline);

                while (it.next()) {
                auto graphics = it.field<Rx::Component::ColorGraphics>(0);
                auto meshes = it.field<Rx::Component::ColorMesh>(1);
                auto transforms = it.field<Rx::Component::Transform>(2);

                    for (auto i : it)
                    {

                        graphics[i].setModelBuffer(transforms[i].getTransformMatrix());

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
                        Rx::Core::colorMeshPipelineLayout,
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
            
            world.system<Rx::Component::ColorMeshArray, Rx::Component::IndirectBuffer, Rx::Component::ColorArrayGraphics>()
            .kind(onRender)
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {
                while(it.next()) {
                    auto colorMeshArray = it.field<Rx::Component::ColorMeshArray>(0);
                    auto indirectBuffer = it.field<Rx::Component::IndirectBuffer>(1);
                    auto graphics = it.field<Rx::Component::ColorArrayGraphics>(2);

                    for( auto i : it) {
                     
                        vkCmdBindDescriptorSets
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        Rx::Core::colorMeshArrayPipelineLayout,
                        0, 1,
                        &graphics[i].vkDescriptorSet, 0, nullptr);

                        vkCmdBindPipeline
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        Rx::Core::colorMeshArrayPipeline);

                        VkDeviceSize offset[] = { 0 };    
                        vkCmdBindVertexBuffers
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        0, 1, 
                        &colorMeshArray[i].vertexBuffer.vkBuffer, offset);

                        vkCmdBindIndexBuffer
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        colorMeshArray[i].indexBuffer.vkBuffer,
                        0, VK_INDEX_TYPE_UINT32);

                        vkCmdDrawIndexedIndirect
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        indirectBuffer[i].buffer.vkBuffer, 0,
                        indirectBuffer[i].numberCommands, 
                        sizeof(VkDrawIndexedIndirectCommand));
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