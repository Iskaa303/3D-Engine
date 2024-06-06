#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"

// std
#include <memory>
#include <vector>

namespace Engine {
class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void run();

    private:
        void loadGameObjects();

        Window engineWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        Device engineDevice{engineWindow};
        Renderer engineRenderer{engineWindow, engineDevice};
        std::vector<GameObject> gameObjects;
};
}  // namespace Engine