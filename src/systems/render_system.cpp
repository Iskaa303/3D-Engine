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
    
struct SimplePushConstantData {
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{};
};

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass)
        : engineDevice{device} {
    createPipelineLayout();
    createPipeline(renderPass);
}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    enginePipeline = std::make_unique<Pipeline>(
        engineDevice,
        "shaders/compiled/shader.vert.spv",
        "shaders/compiled/shader.frag.spv",
        pipelineConfig);
}

void RenderSystem::renderGameObjects(
        VkCommandBuffer commandBuffer,
        std::vector<GameObject>& gameObjects,
        const Camera& camera) {
    enginePipeline->bind(commandBuffer);

    auto projectionView = camera.getProjection() * camera.getView();

    for (auto& obj : gameObjects) {
        SimplePushConstantData push{};
        push.color = obj.color;
        push.transform = projectionView * obj.transform.mat4();

        vkCmdPushConstants(
            commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}
} // namespace Engine