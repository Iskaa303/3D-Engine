#pragma once

#include "camera.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace Engine {
class RenderSystem {
    public:
        RenderSystem(Device &device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

        void renderGameObjects(
            VkCommandBuffer commandBuffer,
            std::vector<GameObject> &gameObjects,
            const Camera &camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        Device &engineDevice;

        std::unique_ptr<Pipeline> enginePipeline;
        VkPipelineLayout pipelineLayout;
};
}  // namespace Engine