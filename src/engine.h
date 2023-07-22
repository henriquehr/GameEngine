#pragma once

#include "device.h"
#include "game_object.h"
#include "model.h"
#include "pipeline.h"
#include "swapchain.h"
#include "window.h"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
    std::unique_ptr<SwapChain> swapChain = nullptr;
    std::unique_ptr<Pipeline> pipeline = nullptr;
    std::vector<GameObject> gameObjects{};

    VkPipelineLayout pipelineLayout{};
    std::vector<VkCommandBuffer> commandBuffers{};

    void sierpinskiTriangle();

    void loadGameObjects();

    void createPipelineLayout();

    void createPipeline();

    void createCommandBuffers();

    void freeCommandBuffers();

    void drawFrame();

    void recreateSwapChain();

    void recordCommandBuffer(int imageIndex);

    void renderGameObjects(VkCommandBuffer commandBuffer);

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
