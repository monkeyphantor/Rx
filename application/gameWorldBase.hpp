 #pragma once

#include "global.hpp"
#include "command.hpp"
#include "renderpass.hpp"
#include "mutex.hpp"
#include "flecs.h"
#include "level.hpp"
#include "VkColorMesh.hpp"
#include "VkColorModelDescriptorSet.hpp"
#include "colorMesh.hpp"
#include "transform.hpp"
#include "VkInstancedColorModelBuffer.hpp"
#include "colorMeshArray.hpp"
#include "colorArrayGraphics.hpp"
#include "indirectBuffer.hpp"
#include "ColorModelInstance.hpp"
#include "VkInstancedColorModelDescriptorSet.hpp"
#include "pipelineLayouts.hpp"
#include "pipelines.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"

namespace Rx{

    struct RenderRunning{};
    struct ShouldBeRendered{};
    struct ShouldBeUpdated{};

    class Application;
    class GameWorldBase {
    public:
        GameWorldBase(Application& app, flecs::world& world) : world(world), app(app) {
        }

        flecs::world& getWorld() {
            return world;
        }

    public:
        virtual void loadGlobal();
        
    protected:
        Application& app;

        flecs::world& world;
        flecs::entity game;

        //render entities
        flecs::entity postUpdate;
        flecs::entity preRender;
        flecs::entity onRenderBegin;
        flecs::entity onRender;
        flecs::entity onRenderEnd;


        flecs::entity colorMeshArrayInstanceRelation;
        flecs::entity instancedColorMeshRelation;

    private:
        void registerComponentsBase();

        void registerObserversBase();

        void registerGraphicsBase();
    };
}