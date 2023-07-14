#pragma once

#include "SDL.h"

class engine {
  private:
    SDL_Window* window;

  public:
    void init();
    void run();
    void cleanup();

  private:
    void initWindow();
};
