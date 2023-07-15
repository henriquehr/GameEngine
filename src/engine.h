#pragma once

#include "pipeline.h"

#include "SDL.h"

class Engine {
  private:
    SDL_Window *window;

    Pipeline pipeline{"../../shaders/compiled/simple.frag.spv", "../../shaders/compiled/simple.vert.spv"};

  public:
    void init();
    void run();
    void cleanup();

  private:
    void initWindow();
};
