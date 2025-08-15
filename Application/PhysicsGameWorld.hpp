#pragma once

#include "GameWorld.hpp"
#include "Physics.hpp"

namespace Rx {
    class Application;
    class PhysicsGameWorld : public GameWorld{
    public:
        PhysicsGameWorld(Application& app, flecs::world& world);

        virtual ~PhysicsGameWorld();

        virtual void loadGlobal() override;

    protected:
        void setupPhysicsWorld();
        void destroyPhysicsWorld();
        void createPhysicsEntities();
        void registerPhysicsComponents();
        void registerPhysicsObservers();
        void registerPhysicsSystems();

        unsigned int maxBodies{65536};
        unsigned int maxBodyPairs{65536};
        unsigned int maxContactConstraints{10240};
        unsigned int numBodyMutexes{0};
        unsigned int tempAllocatorSize{1024 * 1024 * 64}; // 64 MB

        std::unique_ptr<BPLayerInterfaceImpl> broadPhaseLayerInterface{nullptr};
        std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> objectVsBroadPhaseLayerFilter{nullptr};
        std::unique_ptr<ObjectLayerPairFilterImpl> objectLayerPairFilter{nullptr};
        std::unique_ptr<JPH::PhysicsSystem> physicsSystem{nullptr};
        std::unique_ptr<JPH::JobSystemThreadPool> jobSystemThreadPool{nullptr};
        std::unique_ptr<JPH::TempAllocator> tempAllocator{nullptr};
        std::unique_ptr<Rx::FlecsContactListener> contactListener{nullptr};
        
        flecs::entity onPhysicsUpdate;

    public:
        JPH::JobSystemThreadPool& getJobSystem() {
            JPH_ASSERT(jobSystemThreadPool != nullptr);
            return *jobSystemThreadPool;
        }

        JPH::PhysicsSystem& getPhysicsSystem() {
            JPH_ASSERT(physicsSystem != nullptr);
            return *physicsSystem;
        }
   };

}

