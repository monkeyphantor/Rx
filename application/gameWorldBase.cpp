#include "gameWorldBase.hpp"

namespace Rx{

        void GameWorldBase::loadGlobal() {
            world.set_ctx(this);

            registerComponentsBase();
            registerObserversBase();

            game = world.entity("Game");
            game.add<RenderRunning>();

            registerGraphicsBase();

        };

    void GameWorldBase::registerComponentsBase() {
            world.component<RenderRunning>();
            world.component<Rx::Component::Transform>();

            world.component<Rx::Component::VkColorModelDescriptorSet>();
            world.component<Rx::Component::ColorMesh>();
            world.component<Rx::Component::VkColorMesh>();
            world.component<Rx::Component::Transform>();            

            world.component<Rx::Component::MeshArray>();
            world.component<Rx::Component::ColorMeshArray>();
            world.component<Rx::Component::VkInstancedColorModelBuffer>();
            world.component<Rx::Component::IndirectBuffer>();
            world.component<Rx::Component::ColorArrayGraphics>();
            world.component<VkDrawIndexedIndirectCommand>();
        }

        void GameWorldBase::registerObserversBase(){
            world.observer<Rx::Component::VkColorModelBuffer>()
            .event(flecs::OnAdd)
            .each(model_buffer_component_on_add);

            world.observer<Rx::Component::VkColorModelBuffer>()
            .event(flecs::OnRemove)
            .each(model_buffer_component_on_remove);

            world.observer<Rx::Component::VkColorModelBuffer, Rx::Component::VkColorModelDescriptorSet>()
            .event(flecs::OnAdd)
            .each(graphics_component_on_add);
            world.observer<Rx::Component::VkColorModelDescriptorSet>()
            .event(flecs::OnRemove)
            .each(graphics_component_on_remove);

            world.observer<Rx::Component::ColorMesh, Rx::Component::VkColorMesh>()
            .event(flecs::OnAdd)
            .each(mesh_component_on_add);

            world.observer<Rx::Component::VkColorMesh>()
            .event(flecs::OnRemove)
            .each(mesh_component_on_remove);

            world.observer<Rx::Component::MeshArray, Rx::Component::ColorMeshArray>()
            .event(flecs::OnAdd)
            .each(Rx::Component::colorMeshArray_component_on_add);

            world.observer<Rx::Component::ColorMeshArray>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorMeshArray_component_on_remove);

            world.observer<Rx::Component::VkInstancedColorModelBuffer>()
            .event(flecs::OnSet)
            .each(Rx::Component::colorMeshInstanceBuffer_on_set);

            world.observer<Rx::Component::VkInstancedColorModelBuffer>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorMeshInstanceBuffer_on_remove);

            world.observer<Rx::Component::IndirectBuffer>()
            .event(flecs::OnSet)
            .each(Rx::Component::indirectBuffer_component_on_set);

            world.observer<Rx::Component::IndirectBuffer>()
            .event(flecs::OnRemove)
            .each(Rx::Component::indirectBuffer_component_on_remove);

            world.observer<Rx::Component::VkInstancedColorModelBuffer, Rx::Component::ColorArrayGraphics>()
            .event(flecs::OnAdd)
            .each(Rx::Component::colorArrayGraphics_component_on_add);

            world.observer<Rx::Component::ColorArrayGraphics>()
            .event(flecs::OnRemove)
            .each(Rx::Component::colorArrayGraphics_component_on_remove);

            world.observer<Rx::Component::VkInstancedColorModelBuffer, Rx::Component::VkInstancedColorModelDescriptorSet>()
            .event(flecs::OnAdd)
            .each(Rx::Component::instancedColorGraphics_component_on_add);

            world.observer<Rx::Component::VkInstancedColorModelDescriptorSet>()
            .event(flecs::OnRemove)
            .each(Rx::Component::instancedColorGraphics_component_on_remove);
        }

        void GameWorldBase::registerGraphicsBase(){

            
            // Set up render entities
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

            colorMeshArrayInstanceRelation = world.entity("ColorMeshArrayInstanceRelation");
            instancedColorMeshRelation = world.entity("InstancedColorMeshRelation");
            
            world.system<Component::Transform, Component::PointLight>()
            .kind(postUpdate)
            .with<RenderRunning>().src(game)
            .run([&]
            (flecs::iter& it)
            {
                Core::PointLightBuffer& pointLightBuffer = *(Core::PointLightBuffer*)Core::pointLightBuffer.pMemory;
                uint32_t bufferIndex = 0;
                while(it.next()) {
                    uint32_t index = it.count();
                    auto transforms = it.field<Component::Transform>(0);
                    auto lights = it.field<Component::PointLight>(1);

                    for(auto i : it){
                        lights[i].copyTo(transforms[i],pointLightBuffer.lights[bufferIndex++]);
                    }
                }

                pointLightBuffer.numberPointLights[0] = bufferIndex;
            });

            world.system<Component::Transform, Component::DirectionalLight>()
            .kind(postUpdate)
            .with<RenderRunning>().src(game)
            .run([&]
            (flecs::iter& it)
            {
                Core::DirectionalLightBuffer& directionalLightBuffer = *(Core::DirectionalLightBuffer*)Core::directionalLightBuffer.pMemory;
                uint32_t bufferIndex = 0;
                while(it.next()) {
                    uint32_t index = it.count();
                    auto transforms = it.field<Component::Transform>(0);
                    auto lights = it.field<Component::DirectionalLight>(1);

                    for(auto i : it){
                        lights[i].copyTo(transforms[i], directionalLightBuffer.lights[bufferIndex++]);
                    }
                }

                directionalLightBuffer.numberDirectionalLights[0] = bufferIndex;
            });


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

            world.system("VkInstancedColorModelBufferReset")
                .with<Rx::Component::VkInstancedColorModelBuffer>()
                .with<Rx::ShouldBeUpdated>()
                .kind(postUpdate)
                .with<RenderRunning>().src(game)
                .run([](flecs::iter& it) { 
                    while(it.next()) {
                        // This system iterates over all entities with the VkInstancedColorModelBuffer component.
                        auto instanceBuffers = it.field<Rx::Component::VkInstancedColorModelBuffer>(0);
                        for (auto i : it) {
                            instanceBuffers[i].numberInstances = 0; // Reset the instance count for the color mesh instance buffer.
                        }
                    }
                });

            world.system("InstancedColorModelUpdate")
                .with<Rx::Component::Transform>()
                .with<Rx::Component::Material>()
                .with(instancedColorMeshRelation, "$parent")
                .with<ShouldBeUpdated>().src("$parent") 
                .group_by(instancedColorMeshRelation)
                .kind(preRender)
                .with<RenderRunning>().src(game)
                .run([](flecs::iter& it) {

                    uint64_t group_id = 0;
                    uint32_t instance_index = 0;

                    while(it.next()) {
                         if(group_id != it.group_id()) {
                            group_id = it.group_id();
                            instance_index = 0; 
                         }

                        // This system iterates over all entities with the Transform and VkInstancedColorModelBuffer components.
                            flecs::entity parent = it.world().entity(it.group_id());

                            RX_ASSERT(parent.is_alive(), "VkInstancedColorModelBufferUpdate", "System", "Parent entity is not alive");
                            RX_ASSERT(parent.has<Rx::Component::VkInstancedColorModelBuffer>(), "VkInstancedColorModelBufferUpdate", "System", "Parent entity does not have VkInstancedColorModelBuffer component");
                            
                            auto& instance_comp = parent.get_mut<Rx::Component::VkInstancedColorModelBuffer>();
                            auto instance_capacity = instance_comp.maxNumberInstances;
                            Rx::Component::ColorModelInstance* instances = (Rx::Component::ColorModelInstance*) instance_comp.instanceBuffer.pMemory;
                        
                            auto transforms = it.field<Rx::Component::Transform>(0);
                            auto materials = it.field<Rx::Component::Material>(1);

                            for (auto i : it) {

                                if (instance_index >= instance_capacity) {
                                    std::cerr << "Error: buffer overflow for parent "
                                            << parent.name() << "!\n";
                                    break;
                                }
                                auto transform = transforms[i].getTransformMatrix();
                                instances[instance_index].transform = transform;
                                instances[instance_index].normalTransform = glm::transpose(glm::inverse(transform));
                                auto material = materials[i];
                                instances[instance_index].albedo = glm::vec4(material.albedo, 1.0f);
                                instances[instance_index].metalRough = glm::vec4(material.metallic, material.roughness, 0.0f, 0.0f);
                                instances[instance_index].emissive = glm::vec4(material.emissive, 1.0f);

                                instance_index++;
                            }
                            instance_comp.numberInstances = instance_index;

                            parent.remove<ShouldBeUpdated>(); // Remove ShouldBeUpdated component after processing.
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
                        RX_ASSERT(parent.has<Rx::Component::VkInstancedColorModelBuffer>(), "ColorMeshArrayUpdate", "System", "Parent entity does not have VkInstancedColorModelBuffer component");

                        auto& buffer_comp = parent.get_mut<Rx::Component::IndirectBuffer>();
                        auto& instance_comp = parent.get_mut<Rx::Component::VkInstancedColorModelBuffer>();

                        VkDrawIndexedIndirectCommand* commands_dst = (VkDrawIndexedIndirectCommand*) buffer_comp.buffer.pMemory;
                        size_t buffer_capacity = buffer_comp.maxNumberCommands;

                        Rx::Component::ColorModelInstance* instances = (Rx::Component::ColorModelInstance*) instance_comp.instanceBuffer.pMemory;
                        size_t instance_capacity = instance_comp.maxNumberInstances;

                        const auto& src_instances = it.field<const Rx::Component::Transform>(0);
                        const auto& src_command = it.field<const VkDrawIndexedIndirectCommand>(1);

                        uint32_t draw_index = 0;
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
                        instance_comp.numberInstances = draw_index;
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

            world.system<Rx::Component::VkColorModelDescriptorSet, Rx::Component::VkColorModelBuffer, Rx::Component::VkColorMesh, Rx::Component::Material, Rx::Component::Transform>()
            .kind(onRender)
           // .with<ShouldBeRendered>()
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {    
            
                vkCmdBindPipeline
                (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                Rx::Core::colorMeshPipeline);

                while (it.next()) {
                auto descriptorSet = it.field<Rx::Component::VkColorModelDescriptorSet>(0);
                auto modelBuffers = it.field<Rx::Component::VkColorModelBuffer>(1);
                auto meshes = it.field<Rx::Component::VkColorMesh>(2);
                auto materials = it.field<Rx::Component::Material>(3);
                auto transforms = it.field<Rx::Component::Transform>(4);

                    for (auto i : it)
                    {

                        modelBuffers[i].setModelBuffer(transforms[i].getTransformMatrix(), materials[i]);

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
                        &descriptorSet[i].getDescriptorSet(),
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

                vkCmdBindPipeline
                (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                Rx::Core::colorMeshArrayPipeline);

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

            world.system<Rx::Component::VkColorMesh, Rx::Component::VkInstancedColorModelBuffer,Rx::Component::VkInstancedColorModelDescriptorSet>()
            .kind(onRender)
            .with<RenderRunning>().src(game)
            .run([](flecs::iter& it) {
                vkCmdBindPipeline
                (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                Rx::Core::instancedColorMeshPipeline);

                while(it.next()) {
                    auto colorMesh = it.field<Rx::Component::VkColorMesh>(0);
                    auto instanceBuffer = it.field<Rx::Component::VkInstancedColorModelBuffer>(1);
                    auto descriptorSet = it.field<Rx::Component::VkInstancedColorModelDescriptorSet>(2);

                    for( auto i : it) {
                     
                        vkCmdBindDescriptorSets
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        Rx::Core::instancedColorMeshPipelineLayout,
                        0, 1,
                        &descriptorSet[i].vkDescriptorSet, 0, nullptr);

                        VkDeviceSize offset[] = { 0 };    
                        vkCmdBindVertexBuffers
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        0, 1, 
                        &colorMesh[i].vertexBuffer.vkBuffer, offset);

                        vkCmdBindIndexBuffer
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        colorMesh[i].indexBuffer.vkBuffer,
                        0, VK_INDEX_TYPE_UINT32);

                        vkCmdDrawIndexed
                        (Rx::Core::command[Rx::Core::commandIndex].vkCommandBuffer,
                        colorMesh[i].getNumberIndices(),
                        instanceBuffer[i].numberInstances,
                        0, 0, 0);
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

}