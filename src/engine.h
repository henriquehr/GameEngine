#pragma once

#include "device.h"
#include "model.h"
#include "pipeline.h"
#include "swapchain.h"
#include "window.h"

#include "SDL.h"

#include <array>
#include <chrono>
#include <memory>
#include <sstream>
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
    std::unique_ptr<Model> model{};

    VkPipelineLayout pipelineLayout{};
    std::vector<VkCommandBuffer> commandBuffers{};

    void sierpinskiTriangle();

    void loadModels();

    void createPipelineLayout();

    void createPipeline();

    void createCommandBuffers();

    void drawFrame();

  public:
    void init();
    void run();
    void cleanup();

  private:
    // FPS and frame time
    using clock = std::chrono::system_clock;
    std::chrono::time_point<clock> start = clock::now();
    std::chrono::time_point<clock> end{};
    std::chrono::time_point<clock> lastUpdateTime = start;
    int frameCount = 0;
    void fps();
};
