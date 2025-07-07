#include "PhysicsGameWorld.hpp"
#include "application.hpp"
#include "flecs.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "PhysicsBody.hpp"
#include "RigidBody.hpp"
#include "BoxCollider.hpp"

namespace Rx{
    PhysicsGameWorld::PhysicsGameWorld(Application& app, flecs::world& world)
        : GameWorld(app, world) {
    }

    PhysicsGameWorld::~PhysicsGameWorld() {
        destroyPhysicsWorld();
    }

    void PhysicsGameWorld::loadGlobal() {
        GameWorld::loadGlobal();

        setupPhysicsWorld();
        createPhysicsEntities();
        registerPhysicsComponents();
        registerPhysicsObservers();
        registerPhysicsSystems();
    }


    void PhysicsGameWorld::setupPhysicsWorld() {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        broadPhaseLayerInterface = std::make_unique<BPLayerInterfaceImpl>();
        objectVsBroadPhaseLayerFilter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
        objectLayerPairFilter = std::make_unique<ObjectLayerPairFilterImpl>();
        physicsSystem = std::make_unique<JPH::PhysicsSystem>();
        physicsSystem->Init(
            maxBodies,
            numBodyMutexes,
            maxBodyPairs,
            maxContactConstraints,
            *broadPhaseLayerInterface,
            *objectVsBroadPhaseLayerFilter,
            *objectLayerPairFilter
        );

        jobSystemThreadPool = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

        tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(tempAllocatorSize);
    }

    void PhysicsGameWorld::destroyPhysicsWorld() {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void PhysicsGameWorld::createPhysicsEntities() {
        onPhysicsUpdate = world.entity("onPhysicsUpdate");
        onPhysicsUpdate.depends_on(flecs::PreUpdate);

        world.entity(flecs::OnUpdate).depends_on(onPhysicsUpdate);
    }

    void PhysicsGameWorld::registerPhysicsComponents(){
        world.component<Rx::Component::StaticRigidBody>();
        world.component<Rx::Component::DynamicRigidBody>();
        world.component<Rx::Component::BoxCollider>();
        world.component<Rx::Component::PhysicsBody>();
    }

    void PhysicsGameWorld::registerPhysicsObservers() {

        JPH::BodyInterface& bodyInterface = getPhysicsSystem().GetBodyInterface();

        world.observer<Rx::Component::StaticRigidBody, Rx::Component::BoxCollider, Rx::Component::Transform>()
            .event(flecs::OnSet)
            .each([&](flecs::entity e, Rx::Component::StaticRigidBody& rb, Rx::Component::BoxCollider& bc, Rx::Component::Transform& tf) {
                if (e.has<Rx::Component::PhysicsBody>()) return;


                JPH::BodyCreationSettings settings(
                    new JPH::BoxShape(Core::toJoltVec3(bc.halfExtent)),
                    Core::toJoltVec3(tf.translation),
                    Core::toJoltQuat(tf.toRotation()),
                    JPH::EMotionType::Static,
                    rb.objectLayer
                );

                // Set friction and restitution directly from the component
                settings.mFriction = rb.friction;
                settings.mRestitution = rb.restitution;

                // Create the body
                JPH::Body *body = bodyInterface.CreateBody(settings);
                bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

                // Add the JoltBody component to link the entity to the Jolt body.
                // Store the entity's 64-bit ID in Jolt's user data for easy lookup later.
                bodyInterface.SetUserData(body->GetID(), e.id());
                e.set<Component::PhysicsBody>({ body->GetID() });
            });
 
        world.observer<Rx::Component::DynamicRigidBody, Rx::Component::BoxCollider, Rx::Component::Transform>()
            .event(flecs::OnSet)
            .each([&](flecs::entity e, Rx::Component::DynamicRigidBody& rb, Rx::Component::BoxCollider& bc, Rx::Component::Transform& tf) {
                if (e.has<Rx::Component::PhysicsBody>()) return;

                JPH::Ref<JPH::Shape> boxShape = new JPH::BoxShape(Core::toJoltVec3(bc.halfExtent));

                JPH::BodyCreationSettings settings(
                    boxShape,
                    Core::toJoltVec3(tf.translation),
                    Core::toJoltQuat(tf.toRotation()),
                    JPH::EMotionType::Dynamic,
                    rb.objectLayer
                );

                 // Set friction and restitution directly from the component
                settings.mFriction = rb.friction;
                settings.mRestitution = rb.restitution;

                // To set the mass, we need to override the default mass properties.
                // Jolt would otherwise calculate mass and inertia based on shape volume and a default density.
                if (rb.mass > 0.0f) {
                    settings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
                    
                    // GetMassProperties() returns the default mass and inertia for the shape (with density = 1.0).
                    // ScaleToMass() correctly scales both the mass and the inertia tensor to match the desired mass.
                    settings.mMassPropertiesOverride = boxShape->GetMassProperties();
                    settings.mMassPropertiesOverride.ScaleToMass(rb.mass);
                }
                // Create the body
                JPH::Body *body = bodyInterface.CreateBody(settings);
                bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

                // Add the JoltBody component to link the entity to the Jolt body.
                // Store the entity's 64-bit ID in Jolt's user data for easy lookup later.
                bodyInterface.SetUserData(body->GetID(), e.id());
                e.set<Component::PhysicsBody>({ body->GetID() });
            });

        world.observer<Component::PhysicsBody>()
            .event(flecs::OnRemove)
            .each([&](flecs::entity e, Component::PhysicsBody& pb) {
                bodyInterface.RemoveBody(pb.bodyID);
        
                bodyInterface.DestroyBody(pb.bodyID);
            });
    }

    void PhysicsGameWorld::registerPhysicsSystems(){

        world.system("PhysicsUpdate")
            .kind(onPhysicsUpdate)
            .run([&](flecs::iter& it) {
                while(it.next()){
                if(Input::keyX.down){
                    physicsSystem->Update(Time::deltaTime, 1.f/Time::deltaTime > 60.f ? 1 : 2, tempAllocator.get(), jobSystemThreadPool.get());
                    }
                }
                // Update the physics system
            });

        JPH::BodyInterface& bodyInterface = getPhysicsSystem().GetBodyInterface();
                
        world.system<Rx::Component::Transform, Rx::Component::PhysicsBody>()
            .with<Rx::Component::DynamicRigidBody>()
            .kind(onPhysicsUpdate)
            .each([&](flecs::entity e, Rx::Component::Transform& tf, Rx::Component::PhysicsBody& pb) {
                JPH::Vec3 position;
                JPH::Quat rotation;
                bodyInterface.GetPositionAndRotation(pb.bodyID, position, rotation);
                
                tf.translation = Core::toGlmVec3(position);
                auto rot = Core::toGlmQuat(rotation);
                tf.angle = glm::angle(rot);
                tf.axis = glm::axis(rot);

            });
    }
}