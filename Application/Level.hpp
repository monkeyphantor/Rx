#pragma once

#include "../Flecs/include/flecs.h"


namespace Rx {


    class Level {
    public:
        virtual ~Level() = default;
        virtual void onLoad(flecs::world& world) = 0;
        virtual void onUnload(flecs::world& world) = 0;
    };

}