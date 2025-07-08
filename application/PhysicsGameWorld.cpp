#include "PhysicsGameWorld.hpp"
#include "application.hpp"
#include "flecs.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "PhysicsBody.hpp"
#include "RigidBody.hpp"
#include "BoxCollider.hpp"
#include "ContactInfo.hpp"
#include "Velocity.hpp"

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

        contactListener = std::make_unique<FlecsContactListener>();

        physicsSystem->SetContactListener(contactListener.get());
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
 
        world.observer<Rx::Component::DynamicRigidBody, Rx::Component::BoxCollider, Rx::Component::Transform, Rx::Component::Velocity>()
            .event(flecs::OnSet)
            .each([&](flecs::entity e, Rx::Component::DynamicRigidBody& rb, Rx::Component::BoxCollider& bc, Rx::Component::Transform& tf, Rx::Component::Velocity& vel) {
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

                settings.mLinearVelocity = Core::toJoltVec3(vel.velocity);
                settings.mAngularVelocity = Core::toJoltVec3(vel.angularVelocity);

                // Create the body
                JPH::Body *body = bodyInterface.CreateBody(settings);
                bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

                // Add the JoltBody component to link the entity to the Jolt body.
                // Store the entity's 64-bit ID in Jolt's user data for easy lookup later.
                bodyInterface.SetUserData(body->GetID(), e.id());
                e.set<Component::PhysicsBody>({ body->GetID() });
            });

            world.observer<Rx::Component::KinematicRigidBody, Rx::Component::BoxCollider, Rx::Component::Transform>()
            .event(flecs::OnSet)
            .each([&](flecs::entity e, Rx::Component::KinematicRigidBody& rb, Rx::Component::BoxCollider& bc, Rx::Component::Transform& tf) {
                if (e.has<Rx::Component::PhysicsBody>()) return;

                JPH::Ref<JPH::Shape> boxShape = new JPH::BoxShape(Core::toJoltVec3(bc.halfExtent));

                JPH::BodyCreationSettings settings(
                    boxShape,
                    Core::toJoltVec3(tf.translation),
                    Core::toJoltQuat(tf.toRotation()),
                    JPH::EMotionType::Kinematic,
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
            });

        JPH::BodyInterface& bodyInterface = getPhysicsSystem().GetBodyInterface();

        world.system<Rx::Component::Transform, Rx::Component::Velocity, Rx::Component::PhysicsBody>("SyncFlecsFromJolt")
            .with<Rx::Component::DynamicRigidBody>()
            .kind(onPhysicsUpdate)
            .each([&](flecs::entity e, Rx::Component::Transform& tf, Rx::Component::Velocity& vel,Rx::Component::PhysicsBody& pb) {
                JPH::Vec3 position;
                JPH::Quat rotation;
                bodyInterface.GetPositionAndRotation(pb.bodyID, position, rotation);
                
                tf.translation = Core::toGlmVec3(position);
                auto rot = Core::toGlmQuat(rotation);
                tf.angle = glm::angle(rot);
                tf.axis = glm::axis(rot);

                // Update the velocity component
                JPH::Vec3 linearVelocity, angularVelocity;
                bodyInterface.GetLinearAndAngularVelocity(pb.bodyID, linearVelocity, angularVelocity);
                vel.velocity = Core::toGlmVec3(linearVelocity);
                vel.angularVelocity = Core::toGlmVec3(angularVelocity);

            });

        world.system<Rx::Component::Transform, Rx::Component::PhysicsBody>()
            .with<Rx::Component::KinematicRigidBody>()
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

        world.system<Rx::Component::Velocity, Rx::Component::PhysicsBody>()
            .with<Rx::Component::KinematicRigidBody>()
            .kind(onPhysicsUpdate)
            .each([&](flecs::entity e, Rx::Component::Velocity& vel, Rx::Component::PhysicsBody& pb) {
                JPH::Vec3 linearVelocity = Core::toJoltVec3(vel.velocity);
                JPH::Vec3 angularVelocity = Core::toJoltVec3(vel.angularVelocity);
                bodyInterface.SetLinearAndAngularVelocity(pb.bodyID, linearVelocity, angularVelocity);
            });


        world.system<Rx::Component::Transform, Rx::Component::Velocity>("SyncKinematicJoltFromFlecs")
            .with<Rx::Component::PhysicsBody>()
            .with<Rx::Component::KinematicRigidBody>()
            .kind(onPhysicsUpdate)
            .each([&](flecs::entity e, Rx::Component::Transform& tf, Rx::Component::Velocity& vel) {
                tf.translation += vel.velocity * Time::deltaTime;

                glm::vec3 angular_velocity = vel.angularVelocity;
                float angular_speed = glm::length(angular_velocity);

                // Only apply rotation if there is any angular velocity
                if (angular_speed > 0.0001f) {
                    // a. Calculate the rotation for this single frame
                    float angle_delta = angular_speed * Time::deltaTime;
                    glm::vec3 axis = glm::normalize(angular_velocity);

                    // b. Create a "delta" quaternion representing this frame's small rotation
                    glm::quat delta_rotation = glm::angleAxis(angle_delta, axis);

                    // c. Apply the delta rotation to the current rotation.
                    // The order matters! `delta * current` applies the spin in the object's local space.
                    auto rotation = glm::normalize(delta_rotation * glm::angleAxis(tf.angle, tf.axis));

                    // d. Normalize the result to prevent floating-point drift over time
                    tf.angle = glm::angle(rotation);
                    tf.axis = glm::axis(rotation);
                }
            });

         // System to process the collision queue
        world.system("ProcessCollisionEvents")
            .kind(flecs::PreUpdate)
            .run([&](flecs::iter& it) {
                
                while(it.next()) {
                    std::vector<CollisionEvent> events;
                    Core::collisionQueue.drain(events);

                    std::cout << "Processing " << events.size() << " collision events." << std::endl;
                    for (const auto& ev : events) {

                        flecs::entity e1(it.world(), ev.entity1);
                        flecs::entity e2(it.world(), ev.entity2);

                        if (e1.is_valid() && e2.is_valid()) {
                            // This is the key change. We are adding the (ContactInfo, e2) pair to e1,
                            // and storing the contact data *on the relationship instance*.
                            e1.set<Component::ContactInfo>(e2, {ev.contactPoint, ev.contactNormal});

                            // We can do the same for the other direction, but we need to flip the normal.
                            // The normal from Jolt points from B to A. So for the e2->e1 relationship,
                            // the normal is correct as is. For e1->e2, it should be inverted.
                            e2.set<Component::ContactInfo>(e1, {ev.contactPoint, -ev.contactNormal});
                        }
                    }
                }
            });   

        // Cleanup system
        world.system("CleanupCollisions")
            .kind(flecs::PostFrame)
            .run([](flecs::iter& it) {
                while(it.next()){
                    it.world().remove_all<Component::ContactInfo>();
                }
            });
    }
}