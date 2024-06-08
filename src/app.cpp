#include "app.hpp"

#include "input_controller.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "systems/render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace Engine {
struct GlobalUbo {
    glm::mat4 projectionView{1.f};
    glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
};

App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
            engineDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    RenderSystem engineRenderSystem{engineDevice, engineRenderer.getSwapChainRenderPass()};
    Camera camera{};
    
    auto viewerObject = GameObject::createGameObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!engineWindow.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        frameTime = glm::min(frameTime, MAX_FRAME_RATE);

        cameraController.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, viewerObject);
        cameraController.handleMouseInput(engineWindow.getGLFWwindow(), viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = engineRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 25.f);

        if (auto commandBuffer = engineRenderer.beginFrame()) {
            int frameIndex = engineRenderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};

            // update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
            engineRenderer.beginSwapChainRenderPass(commandBuffer);
            engineRenderSystem.renderGameObjects(frameInfo, gameObjects);
            engineRenderer.endSwapChainRenderPass(commandBuffer);
            engineRenderer.endFrame();
        }
    }
    vkDeviceWaitIdle(engineDevice.device());
}

void App::loadGameObjects() {
    std::shared_ptr<Model> engineModel =
        Model::createModelFromFile(engineDevice, "models/flat_vase.obj");
    auto flatVase = GameObject::createGameObject();
    flatVase.model = engineModel;
    flatVase.transform.translation = {-.5f, .5f, 2.5f};
    flatVase.transform.scale = {3.f, 1.5f, 3.f};
    gameObjects.push_back(std::move(flatVase));

    engineModel = Model::createModelFromFile(engineDevice, "models/smooth_vase.obj");
    auto smoothVase = GameObject::createGameObject();
    smoothVase.model = engineModel;
    smoothVase.transform.translation = {.5f, .5f, 2.5f};
    smoothVase.transform.scale = {3.f, 1.5f, 3.f};
    gameObjects.push_back(std::move(smoothVase));
}
} // namespace Engine