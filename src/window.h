#pragma once

#include "SDL.h"

class Window {
  public:
    Window();
    ~Window();

  private:
    SDL_Window *window{};

    void initWindow();

  public:
    SDL_Window *getSDLWindow();
};
