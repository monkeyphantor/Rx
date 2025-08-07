#include "gameWorld.hpp"

namespace Rx{
        GameWorld::GameWorld(Application& app, flecs::world& world): GameWorldBase(app, world) {
        }

        void GameWorld::loadGlobal(){
            GameWorldBase::loadGlobal();
            registerComponents();

           // world.import<flecs::stats>();

            // flecs::Rest rest;
            // rest.port = 27750;
            // rest.ipaddr = (char*) malloc(10 * sizeof(char));
            // strcpy(rest.ipaddr, "127.0.0.1");

            // world.set<flecs::Rest>(rest);

            //world.set<flecs::Rest>({});
        
        };




        bool GameWorld::getShouldSwitch() const {
            return shouldSwitch;
        }

        bool GameWorld::getShouldQuit() const {
            return shouldQuit;
        }

        std::atomic<bool>& GameWorld::getIsLoadComplete() {
            return isLoadComplete;
        }

        std::thread& GameWorld::getLoadingThread() {
            return loadingThread;
        }

        void GameWorld::registerComponents() {
            // Register components here
            world.component<LevelAsset>();
        }

        void GameWorld::beginInitialLevelLoad(){
            shouldSwitch = false;

            // 1. Reset the completion flag.
            isLoadComplete.store(false);

            // 2. Ensure previous thread is cleaned up before starting a new one.
            if (loadingThread.joinable()) {
                loadingThread.join();
            }

            // 3. Launch the background thread to do the actual work.
            std::cout << "[Main Thread] Starting background thread for initial level loading..." << std::endl;
            loadingThread = std::thread(&GameWorld::loadInitialLevelAsync, this, nextLevelFactory);
        }

        void GameWorld::beginLevelLoad() {
            shouldSwitch = false;

            // 2. Reset the completion flag.
            isLoadComplete.store(false);

            // 3. Ensure previous thread is cleaned up before starting a new one.
            if (loadingThread.joinable()) {
                loadingThread.join();
            }

            // 4. Launch the background thread to do the actual work.
            std::cout << "[Main Thread] Starting background thread for level loading..." << std::endl;
            loadingThread = std::thread(&GameWorld::loadLevelAsync, this, nextLevelFactory);
        }

        // This function runs on the background thread.
        void GameWorld::loadInitialLevelAsync(std::function<std::unique_ptr<Level>()> levelFactory) {
            std::cout << "[Background Thread " << std::this_thread::get_id() << "] Starting level asset operations." << std::endl;
            
            this->loadGlobal();

            // 3. Create and load the new level.
            std::unique_ptr<Level> newLevel = levelFactory();
            newLevel->onLoad(world);
            m_currentLevel = std::move(newLevel);
            
            std::cout << "\n[Background Thread " << std::this_thread::get_id() << "] New level loaded into Game World." << std::endl;

            // 4. Signal to the main thread that we are done.
            isLoadComplete.store(true);
        }

        // This function runs on the background thread.
        void GameWorld::loadLevelAsync(std::function<std::unique_ptr<Level>()> levelFactory) {
            std::cout << "[Background Thread " << std::this_thread::get_id() << "] Starting level asset operations." << std::endl;

            // 1. Destroy all old level assets in the game world.
            // This is safe because the main thread is not touching m_gameWorld.
            world.defer_begin();
            world.query_builder()
            .with<LevelAsset>()
            .each([](flecs::entity e) {
                e.destruct();
            });
            world.defer_end();
            std::cout << "[Background Thread " << std::this_thread::get_id() << "] Old assets cleared." << std::endl;

            // 3. Create and load the new level.
            std::unique_ptr<Level> newLevel = levelFactory();
            newLevel->onLoad(world);
            m_currentLevel = std::move(newLevel);
            
            std::cout << "\n[Background Thread " << std::this_thread::get_id() << "] New level loaded into Game World." << std::endl;

            // 4. Signal to the main thread that we are done.
            isLoadComplete.store(true);
        }
}