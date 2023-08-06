#pragma once

#include "../camera.h"
#include "../frame_info.h"
#include "../game_object.h"
#include "../vulkan/device.h"
#include "../vulkan/pipeline.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <memory>

class PointLightSystem {
  public:
    PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem &) = delete;
    PointLightSystem &operator=(const PointLightSystem &) = delete;

    void update(FrameInfo frameInfo, GlobalUbo &ubo);

    void render(FrameInfo frameInfo);

  private:
    Device &device;
    std::unique_ptr<Pipeline> pipeline = nullptr;
    VkPipelineLayout pipelineLayout{};

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

    void createPipeline(VkRenderPass renderPass);
};
