#include <iostream>
#include <memory>
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

// Include the flecs header
#include "flecs.h"

// --- Components and Tags (Mostly unchanged) ---

struct LevelAsset {};
struct PlayerData { int score; int lives; };
struct Position { float x, y; };
struct LevelEndPortal {};

// --- The Application Class (Multi-World Version) ---

class Application {
public:
    // Application state to control the main loop
    enum class State {
        LOADING,
        RUNNING_GAME,
        QUITTING
    };

    // Level class remains the same abstract interface
    class Level {
    public:
        virtual ~Level() = default;
        virtual void onLoad(flecs::world& world) = 0;
        
        static Application* getContext(const flecs::iter& it) {
            // Note: This now assumes the context is the same for both worlds
            return it.world().ctx<Application>();
        }
    };

    Application() : 
        m_state(State::LOADING), 
        m_shouldSwitch(false), 
        m_isLoadComplete(false)
    {
        // Set the application instance as a context on BOTH worlds.
        // This allows systems from either world to interact with the Application.
        m_gameWorld.set_ctx(this);
        m_loadingWorld.set_ctx(this);

        // --- Setup the Loading Screen World ---
        // This is done once at the start.
        m_loadingWorld.entity("LoadingText")
            .set<Position>({ 10, 10 });

        // A simple system to animate the loading screen.
        m_loadingWorld.system("AnimateLoading")
            .kind(flecs::OnUpdate)
            .iter([](flecs::iter& it) {
                static int frame_count = 0;
                std::string dots;
                for (int i = 0; i < (frame_count / 30) % 4; ++i) {
                    dots += ".";
                }
                std::cout << "[Loading Screen Thread " << std::this_thread::get_id() << "] Loading" << dots << "\r" << std::flush;
                frame_count++;
            });
    }

    ~Application() {
        // Ensure the background thread is joined if the app closes while loading.
        if (m_loadingThread.joinable()) {
            m_loadingThread.join();
        }
    }

    // The main application loop, now a state machine.
    void run() {
        std::cout << "--- Application starting ---" << std::endl;

        while (m_state != State::QUITTING) {
            switch (m_state) {
                case State::LOADING:
                    runLoading();
                    break;
                case State::RUNNING_GAME:
                    runGame();
                    break;
                case State::QUITTING:
                    // Loop will terminate
                    break;
            }
        }
        
        std::cout << "\n--- Application ended ---" << std::endl;
    }

    // This is called by a system in the GAME world to request a level change.
    template <typename T>
    void requestSwitch() {
        static_assert(std::is_base_of<Level, T>::value, "T must be a subclass of Application::Level");
        std::cout << "\n[Game World] Level switch requested to " << typeid(T).name() << std::endl;

        m_nextLevelFactory = []() { return std::make_unique<T>(); };
        m_shouldSwitch = true;
    }

    // Sets the very first level to load.
    template <typename T>
    void setInitialLevel() {
        requestSwitch<T>();
        beginLevelLoad();
    }

private:
    // --- State-specific loop functions ---

    void runLoading() {
        // Progress the loading screen world.
        m_loadingWorld.progress();
        
        // Check if the background thread has finished loading the game world.
        if (m_isLoadComplete.load()) {
            std::cout << "\n[Main Thread] Load complete! Joining background thread..." << std::endl;
            m_loadingThread.join(); // Clean up the finished thread.

            std::cout << "[Main Thread] Switching to Game World." << std::endl;
            m_state = State::RUNNING_GAME;
        }
    }

    void runGame() {
        // Progress the main game world.
        m_gameWorld.progress();

        // Check for quit signals from systems.
        if (!m_gameWorld.is_running()) {
            m_state = State::QUITTING;
            return;
        }
        
        // After a frame, check if a level switch was requested by a game system.
        if (m_shouldSwitch) {
            beginLevelLoad();
        }
    }

    // --- Threaded loading logic ---

    void beginLevelLoad() {
        // 1. Set the state to LOADING. The main loop will now run runLoading().
        m_state = State::LOADING;
        m_shouldSwitch = false;

        // 2. Reset the completion flag.
        m_isLoadComplete.store(false);

        // 3. Ensure previous thread is cleaned up before starting a new one.
        if (m_loadingThread.joinable()) {
            m_loadingThread.join();
        }

        // 4. Launch the background thread to do the actual work.
        std::cout << "[Main Thread] Starting background thread for level loading..." << std::endl;
        m_loadingThread = std::thread(&Application::loadLevelAsync, this, m_nextLevelFactory);
    }

    // This function runs on the background thread.
    void loadLevelAsync(std::function<std::unique_ptr<Level>()> levelFactory) {
        std::cout << "[Background Thread " << std::this_thread::get_id() << "] Starting level asset operations." << std::endl;

        // 1. Destroy all old level assets in the game world.
        // This is safe because the main thread is not touching m_gameWorld.
        m_gameWorld.defer_begin();
        m_gameWorld.filter<LevelAsset>().each([](flecs::entity e) {
            e.destruct();
        });
        m_gameWorld.defer_end();
        std::cout << "[Background Thread " << std::this_thread::get_id() << "] Old assets cleared." << std::endl;

        // 2. Simulate a long loading time.
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // 3. Create and load the new level.
        std::unique_ptr<Level> newLevel = levelFactory();
        newLevel->onLoad(m_gameWorld);
        m_currentLevel = std::move(newLevel);
        
        std::cout << "\n[Background Thread " << std::this_thread::get_id() << "] New level loaded into Game World." << std::endl;

        // 4. Signal to the main thread that we are done.
        m_isLoadComplete.store(true);
    }

    // --- Member Variables ---
    flecs::world m_gameWorld;
    flecs::world m_loadingWorld;
    
    std::unique_ptr<Level> m_currentLevel;

    // State management
    State m_state;
    bool m_shouldSwitch;

    // Threading
    std::thread m_loadingThread;
    std::atomic<bool> m_isLoadComplete;
    std::function<std::unique_ptr<Level>()> m_nextLevelFactory;
};


// --- Concrete Level Implementations (Identical to previous example) ---

class GameLevel;

class MainMenu : public Application::Level {
public:
    void onLoad(flecs::world& world) override {
        std::cout << "[Background Thread " << std::this_thread::get_id() << "] Loading MainMenu..." << std::endl;
        world.entity("StartButton").add<LevelAsset>();
        world.system("CheckStartGame")
            .add<LevelAsset>()
            .interval(1.0) // Run once a second
            .iter([](flecs::iter& it) {
                // To prevent instant switching, we wait a bit.
                if (it.world().get_info()->world_time_total > 1.0) {
                     Application::getContext(it)->requestSwitch<GameLevel>();
                }
            });
    }
};

class GameLevel : public Application::Level {
public:
    void onLoad(flecs::world& world) override {
        std::cout << "[Background Thread " << std::this_thread::get_id() << "] Loading GameLevel..." << std::endl;
        world.entity("Enemy").add<LevelAsset>().set<Position>({50, 50});
        world.entity("Portal").add<LevelAsset>().add<LevelEndPortal>();
        
        world.system("TriggerWinCondition")
            .add<LevelAsset>()
            .interval(2.0)
            .iter([](flecs::iter& it) {
                if (it.world().get_info()->world_time_total > 2.0) {
                    std::cout << "\n[Game System] Win condition met!";
                    auto* pdata = it.world().get_mut<PlayerData>();
                    if (pdata) pdata->score += 100;
                    
                    // Request switch back to the menu
                    Application::getContext(it)->requestSwitch<MainMenu>();

                    // For this demo, we'll quit after one full cycle.
                    it.world().quit();
                }
            });
    }
};

// --- Main Function ---

int main() {
    Application app;

    // Setup global state in the game world. This will persist.
    // We get a direct reference because we know 'app' owns the world.
    flecs::world& gameWorld = app.getGameWorld(); // A helper to get the world would be nice
    gameWorld.entity("GlobalPlayerState").set<PlayerData>({0, 3});

    // Start the entire process by loading the Main Menu.
    app.setInitialLevel<MainMenu>();
    
    // This will now handle both loading and game states.
    app.run();

    // Verify persistence
    std::cout << "--- Post-Run Verification ---" << std::endl;
    const PlayerData* pdata = gameWorld.get<PlayerData>();
    if (pdata) {
        std::cout << "Final player score: " << pdata->score << std::endl;
    }
    
    return 0;
}