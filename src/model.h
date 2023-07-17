#pragma once

#include "device.h"

#include <glm/glm.hpp>

#include <cassert>
#include <vector>

class Model {
  public:
    struct Vertex {
        glm::vec2 position;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    Model(Device &device, const std::vector<Vertex> &vertices);
    ~Model();

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

  private:
    Device &device;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;

    void createVertexBuffer(const std::vector<Vertex> &vertices);
};
