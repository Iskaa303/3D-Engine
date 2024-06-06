#pragma once

#include <memory>
#include "model.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Engine {
std::unique_ptr<Model> createSquareModel(Device& device, glm::vec2 offset) {
    std::vector<Model::Vertex> vertices = {
        {{-0.5f, -0.5f}},
        {{0.5f, 0.5f}},
        {{-0.5f, 0.5f}},
        {{-0.5f, -0.5f}},
        {{0.5f, -0.5f}},
        {{0.5f, 0.5f}},
    };
    for (auto& v : vertices) {
        v.position += offset;
    }
    return std::make_unique<Model>(device, vertices);
}
 
std::unique_ptr<Model> createCircleModel(Device& device, unsigned int numSides) {
    std::vector<Model::Vertex> uniqueVertices{};
    for (int i = 0; i < numSides; i++) {
        float angle = i * glm::two_pi<float>() / numSides;
        uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
    }
    uniqueVertices.push_back({});  // adds center vertex at 0, 0
    
    std::vector<Model::Vertex> vertices{};
    for (int i = 0; i < numSides; i++) {
        vertices.push_back(uniqueVertices[i]);
        vertices.push_back(uniqueVertices[(i + 1) % numSides]);
        vertices.push_back(uniqueVertices[numSides]);
    }
    return std::make_unique<Model>(device, vertices);
}
} //namespace Engine