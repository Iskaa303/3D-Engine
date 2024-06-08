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
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

        if (auto commandBuffer = engineRenderer.beginFrame()) {
            engineRenderer.beginSwapChainRenderPass(commandBuffer);

            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

            engineRenderer.endSwapChainRenderPass(commandBuffer);
            engineRenderer.endFrame();
        }
    }
    vkDeviceWaitIdle(engineDevice.device());
}

// temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
    Model::Builder modelBuilder{};
    modelBuilder.vertices = {
    // left face (white)
    {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

    // right face (yellow)
    {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

    // top face (orange, remember y axis points down)
    {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

    // bottom face (red)
    {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

    // nose face (blue)
    {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

    // tail face (green)
    {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices) {
        v.position += offset;
    }
    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<Model>(device, modelBuilder);
}

void App::loadGameObjects() {
    std::shared_ptr<Model> engineModel = createCubeModel(engineDevice, {.0f, .0f, .0f});
    auto cube = GameObject::createGameObject();
    cube.model = engineModel;
    cube.transform.translation = {.0f, .0f, .5f};
    cube.transform.scale = {.5f, .5f, .5f};
    gameObjects.push_back(std::move(cube));
}
} // namespace Engine