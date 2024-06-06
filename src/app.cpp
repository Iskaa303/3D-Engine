#include "app.hpp"

#include "render_system.hpp"

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
    RenderSystem simpleRenderSystem{engineDevice, engineRenderer.getSwapChainRenderPass()};

    while (!engineWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = engineRenderer.beginFrame()) {
            engineRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            engineRenderer.endSwapChainRenderPass(commandBuffer);
            engineRenderer.endFrame();
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