#include "Application/Application.hpp"
#include "RxLoadingScreen.hpp"
#include "Application/GameWorldBase.hpp"
#include "RxGameWorld.hpp"
#include "RxLevel.hpp"
using namespace Rx;

int main(){
    Rx::Core::windowTitle = "RxWorld";
    Rx::Core::windowHeight = 600;
    Rx::Core::windowWidth = 1200;
    Rx::Core::vkSampleCountFlagBits = VK_SAMPLE_COUNT_4_BIT;

    {
        Rx::Application app;

        app
        .addLoadingWorld<Rx::RxLoadingScreen>()
        .addGameWorld<Rx::RxGameWorld, Rx::RxLevel>();

        app.run();
    }

}  
