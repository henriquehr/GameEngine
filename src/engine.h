#pragma once

#include "device.h"
#include "pipeline.h"
#include "window.h"

#include "SDL.h"

class Engine {
  private:
    int WIDTH = 1600;
    int HEIGHT = 900;

    Window window{WIDTH, HEIGHT};

    Device device{window};

    Pipeline pipeline{device, Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT), "../../shaders/compiled/simple.frag.spv",
                      "../../shaders/compiled/simple.vert.spv"};

  public:
    void init();
    void run();
    void cleanup();
};
