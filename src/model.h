#pragma once

#include "device.h"

#include <glm/glm.hpp>

#include <cassert>
#include <vector>

class Model {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Data {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
    };

    Model(Device &device, const Model::Data &data);
    ~Model();

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

  private:
    Device &device;

    VkBuffer vertexBuffer{};
    VkDeviceMemory vertexBufferMemory{};
    uint32_t vertexCount{};

    bool hasIndexBuffer = false;
    VkBuffer indexBuffer{};
    VkDeviceMemory indexBufferMemory{};
    uint32_t indexCount{};

    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffer(const std::vector<uint32_t> &indices);
};
