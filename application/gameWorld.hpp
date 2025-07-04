  #pragma once

#include "gameWorldBase.hpp"

struct LevelAsset {};

namespace Rx{
    class Application;
    class GameWorld : public GameWorldBase {
    public:
        GameWorld(Application& app, flecs::world& world);
    public:
        virtual void loadGlobal() override;

    public:
        template <typename T>
        void requestLevelSwitch();

                // This is called by a system in the GAME world to request a level change.
        template <typename T>
        void requestSwitch();

        template <typename T>
        void requestInitialLevel();


        bool getShouldSwitch() const ;

        bool getShouldQuit() const ;

        std::atomic<bool>& getIsLoadComplete() ;

        std::thread& getLoadingThread() ;

        std::unique_ptr<Level> m_currentLevel;
        std::function<std::unique_ptr<Level>()> nextLevelFactory{ nullptr };
       
        void beginLevelLoad() ;

    private:
        void registerComponents();

        bool shouldSwitch{ false };
        bool shouldQuit{ false };

        // Threading
        std::thread loadingThread;
        std::atomic<bool> isLoadComplete;


        void beginInitialLevelLoad();

protected:
        // This function runs on the background thread.
        void loadInitialLevelAsync(std::function<std::unique_ptr<Level>()> levelFactory);
        // This function runs on the background thread.
        void loadLevelAsync(std::function<std::unique_ptr<Level>()> levelFactory);
    };


}

namespace Rx{
            template <typename T>
        void GameWorld::requestLevelSwitch(){
            static_assert(std::is_base_of<Rx::Level, T>::value, "T must be a subclass of Rx::Level");
            std::cout << "\n[Game World] Level switch requested to " << typeid(T).name() << std::endl;

            nextLevelFactory = []() { return std::make_unique<T>(); };
            shouldSwitch = true;
        }

        template <typename T>
        void GameWorld::requestInitialLevel(){
            static_assert(std::is_base_of<Level, T>::value, "T must be a subclass of Application::Level");

            std::cout << "\n[Game World] Initial level requested to " << typeid(T).name() << std::endl;
            nextLevelFactory = []() { return std::make_unique<T>(); };
            beginInitialLevelLoad();
        }
}