#pragma once

#include "../camera.h"
#include "../device.h"
#include "../frame_info.h"
#include "../game_object.h"
#include "../pipeline.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <memory>

// pipeline and its layout, and data to render game objects
class TextureRenderSystem {
  public:
    TextureRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~TextureRenderSystem();

    TextureRenderSystem(const TextureRenderSystem &) = delete;
    TextureRenderSystem &operator=(const TextureRenderSystem &) = delete;

    void renderGameObjects(FrameInfo frameInfo);

  private:
    Device &device;
    std::unique_ptr<Pipeline> pipeline = nullptr;
    VkPipelineLayout pipelineLayout{};

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

    void createPipeline(VkRenderPass renderPass);
};
