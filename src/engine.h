#pragma once

#include "device.h"
#include "game_object.h"
#include "model.h"
#include "renderer.h"
#include "simple_render_system.h"
#include "window.h"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
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
    Renderer renderer{window, device};
    std::vector<GameObject> gameObjects{};

    void sierpinskiTriangle();

    void loadGameObjects();

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
