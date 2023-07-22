#pragma once

#include "device.h"
#include "game_object.h"
#include "model.h"
#include "pipeline.h"
#include "renderer.h"
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
    Renderer renderer{window, device};
    std::unique_ptr<Pipeline> pipeline = nullptr;
    std::vector<GameObject> gameObjects{};

    VkPipelineLayout pipelineLayout{};

    void sierpinskiTriangle();

    void loadGameObjects();

    void createPipelineLayout();

    void createPipeline();

    void renderGameObjects(VkCommandBuffer commandBuffer);

  public:
    void run();

  private:
    // FPS and frame time
    using clock = std::chrono::system_clock;
    std::chrono::time_point<clock> start = clock::now();
    std::chrono::time_point<clock> end{};
    std::chrono::time_point<clock> lastUpdateTime = start;
    int frameCount = 0;
    void fps();
};
