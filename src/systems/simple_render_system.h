#pragma once

#include "../camera.h"
#include "../frame_info.h"
#include "../game_object.h"
#include "../vulkan/device.h"
#include "../vulkan/pipeline.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <memory>

// pipeline and its layout, and data to render game objects
class SimpleRenderSystem {
  public:
    SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(FrameInfo frameInfo);

  private:
    Device &device;
    std::unique_ptr<Pipeline> pipeline = nullptr;
    VkPipelineLayout pipelineLayout{};

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

    void createPipeline(VkRenderPass renderPass);
};
