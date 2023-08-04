#pragma once

#include "buffer.h"
#include "device.h"
#include "texture.h"
#include "utils.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

class Model {
  public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const Vertex &other) const {
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
        }
    };

    struct Data {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void loadModel(const std::string &filePath);
    };

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    Model(Device &device, const Model::Data &data);
    ~Model();

    static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filePath);

    void bind(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer);

    void setTexture(std::shared_ptr<Texture> texture) {
        this->texture = std::move(texture);
    }

    std::shared_ptr<Texture> getTexture() {
        return texture;
    }

  private:
    Device &device;

    std::unique_ptr<Buffer> vertexBuffer{};
    uint32_t vertexCount{};

    bool hasIndexBuffer = false;
    std::unique_ptr<Buffer> indexBuffer{};
    uint32_t indexCount{};

    std::shared_ptr<Texture> texture{};

    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffer(const std::vector<uint32_t> &indices);
};
