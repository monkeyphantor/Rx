 #pragma once

#include "global.hpp"
#include "command.hpp"
#include "renderpass.hpp"
#include "mutex.hpp"
#include "flecs.h"
#include "level.hpp"

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
        flecs::entity textureModelInstanceRelation;
        flecs::entity IsSkeletonModelInstanceOf;
        flecs::entity IsAnimationOf;
        flecs::entity IsAnimationInstanceOf;
        flecs::entity IsRootNodeOf;
        flecs::entity IsRootNodeInstanceOf;
        flecs::entity IsChildNodeOf;
        flecs::entity IsChildNodeInstanceOf;


    private:
        void registerComponentsBase();

        void registerObserversBase();

        void registerGraphicsBase();
    };
}