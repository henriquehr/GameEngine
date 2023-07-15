#pragma once

#include "pipeline.h"
#include "window.h"

#include "SDL.h"

class Engine {
  private:
    Window window{};

    Pipeline pipeline{"../../shaders/compiled/simple.frag.spv", "../../shaders/compiled/simple.vert.spv"};

  public:
    void init();
    void run();
    void cleanup();
};
