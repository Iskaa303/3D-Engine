#pragma once

#include "camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace Engine {
struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera &camera;
};
}  // namespace Engine