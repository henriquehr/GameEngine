#pragma once

#include "device.h"
#include "game_object.h"
#include "pipeline.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

// pipeline and its layout, and data to render game objects
class SimpleRenderSystem {
  public:
    SimpleRenderSystem(Device &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects);

  private:
    Device &device;
    std::unique_ptr<Pipeline> pipeline = nullptr;
    VkPipelineLayout pipelineLayout{};

    void createPipelineLayout();

    void createPipeline(VkRenderPass renderPass);
};
