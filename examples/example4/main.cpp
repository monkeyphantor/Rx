#include "Level1.hpp"
#include "application.hpp"
#include "RxDefaultLoadingScreen.hpp"
#include "GameWorldExample4.hpp"

using namespace Rx;

int main(){
    Rx::Core::windowTitle = "RxWorld";
    Rx::Core::windowHeight = 600;
    Rx::Core::windowWidth = 1200;
    Rx::Core::vkSampleCountFlagBits = VK_SAMPLE_COUNT_4_BIT;

    {
        Rx::Application app;

        app
        .addLoadingWorld<Rx::RxDefaultLoadingScreen>()
        .addGameWorld<GameWorldExample4, Rx::RxLevel1>();

        app.run();
    }

}  