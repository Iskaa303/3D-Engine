#include "app.hpp"

#include "input_controller.hpp"
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
App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
    RenderSystem simpleRenderSystem{engineDevice, engineRenderer.getSwapChainRenderPass()};
    Camera camera{};
    //GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //glfwSetCursor(engineWindow.getGLFWwindow(), cursor);
    
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
            engineRenderer.beginSwapChainRenderPass(commandBuffer);

            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

            engineRenderer.endSwapChainRenderPass(commandBuffer);
            engineRenderer.endFrame();
        }
    }
    vkDeviceWaitIdle(engineDevice.device());
}

void App::loadGameObjects() {
    std::shared_ptr<Model> engineModel =
        Model::createModelFromFile(engineDevice, "models/smooth_vase.obj");
    auto gameObj = GameObject::createGameObject();
    gameObj.model = engineModel;
    gameObj.transform.translation = {.0f, .0f, 2.5f};
    gameObj.transform.scale = glm::vec3(3.f);
    gameObjects.push_back(std::move(gameObj));
}
} // namespace Engine