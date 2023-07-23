#pragma once

#include "camera.h"
#include "device.h"
#include "game_object.h"
#include "keyboard_movement_controller.h"
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

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

  private:
    int WIDTH = 1600;
    int HEIGHT = 900;

    Window window{WIDTH, HEIGHT};
    Device device{window};
    Renderer renderer{window, device};
    std::vector<GameObject> gameObjects{};

    void loadGameObjects();

  public:
    void run();

  private:
    // FPS and frame time
    using clock = std::chrono::high_resolution_clock;
    float deltaTime = 0.0f;
    std::chrono::time_point<clock> currentTime = clock::now();
    std::chrono::time_point<clock> lastUpdateTime = clock::now();
    int frameCount = 0;

    void fps();
};
