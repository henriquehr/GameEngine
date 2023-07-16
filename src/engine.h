#pragma once

#include "device.h"
#include "pipeline.h"
#include "swapchain.h"
#include "window.h"

#include "SDL.h"

#include <array>
#include <memory>
#include <stdexcept>
#include <vector>

class Engine {
  public:
    Engine();
    ~Engine();

  private:
    int WIDTH = 1600;
    int HEIGHT = 900;

    Window window{WIDTH, HEIGHT};
    Device device{window};
    Swapchain swapchain{device, window.getExtent()};
    std::unique_ptr<Pipeline> pipeline{};

    VkPipelineLayout pipelineLayout{};
    std::vector<VkCommandBuffer> commandBuffers{};

    void createPipelineLayout();

    void createPipeline();

    void createCommandBuffers();

    void drawFrame();

  public:
    void init();
    void run();
    void cleanup();
};
