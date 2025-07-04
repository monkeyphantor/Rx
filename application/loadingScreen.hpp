#pragma once
#include "gameWorldBase.hpp"

namespace Rx {

    class Application;

    class LoadingScreen : public Rx::GameWorldBase {
    public:
        LoadingScreen(Application& app, flecs::world& world) : Rx::GameWorldBase(app, world) {
        }
        virtual ~LoadingScreen() = default;
    protected:

    public:
        virtual void loadGlobal() override {
            GameWorldBase::loadGlobal();
            registerComponents();
        }
    private:
        void registerComponents() {
            
        }

        void registerSystem(){
            
        }

};

} // namespace Rx