#include "app.hpp"

#include "render_system.hpp"
#include "rainbow_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace Engine {
App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
    RenderSystem engineRenderSystem{engineDevice, engineRenderer.getSwapChainRenderPass()};
    RainbowSystem engineRainbowSystem{1000.0f};

    while (!engineWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = engineRenderer.beginFrame()) {
            engineRenderer.beginSwapChainRenderPass(commandBuffer);
            engineRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            engineRenderer.endSwapChainRenderPass(commandBuffer);
            engineRenderer.endFrame();

            engineRainbowSystem.update(50.0f, gameObjects);
        }
    }

    vkDeviceWaitIdle(engineDevice.device());
}

void App::loadGameObjects() {

    std::vector<Model::Vertex> vertices{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    auto engineModel = std::make_shared<Model>(engineDevice, vertices);

    auto triangle = GameObject::createGameObject();
    triangle.model = engineModel;
    triangle.color = {0.1f, 0.8f, 0.1f};
    triangle.transform2d.translation.x = 0.2f;
    triangle.transform2d.scale = {2.0f, 0.5f};
    triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();
    
    gameObjects.push_back(std::move(triangle));
}
}  // namespace Engine