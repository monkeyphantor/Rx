#pragma once
#include "gameWorldBase.hpp"
#include "../component/colorMesh.hpp"
#include "../component/colorGraphics.hpp"
#include "../component/transform.hpp"

namespace Rx {

    class Application;

    class LoadingScreen : public GameWorldBase {
    public:
        LoadingScreen(Application& app, flecs::world& world) : GameWorldBase(app, world) {
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