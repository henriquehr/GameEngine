#pragma once

#include "buffer.h"
#include "camera.h"
#include "descriptors.h"
#include "device.h"
#include "game_object.h"
#include "keyboard_movement_controller.h"
#include "model.h"
#include "renderer.h"
#include "systems/point_light_system.h"
#include "systems/simple_render_system.h"
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

    void run();

  private:
    int WIDTH = 1600;
    int HEIGHT = 900;

    Window window{WIDTH, HEIGHT};
    Device device{window};
    Renderer renderer{window, device};

    std::unique_ptr<DescriptorPool> globalPool{};
    GameObject::Map gameObjects{};

    void loadGameObjects();

    // FPS and frame time
    using clock = std::chrono::system_clock;
    float deltaTime = 0.0f;
    std::chrono::time_point<clock> currentTime = clock::now();
    std::chrono::time_point<clock> lastUpdateTime = clock::now();
    int frameCount = 0;

    void fps();
};
