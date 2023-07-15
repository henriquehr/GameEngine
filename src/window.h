#pragma once

#include "SDL.h"
#include "SDL_vulkan.h"
#include "vulkan/vulkan.h"

#include "stdexcept"

class Window {
  public:
    Window();
    ~Window();

    SDL_Window *getSDLWindow() {
        return window;
    }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    SDL_Window *window{};

    void initWindow();
};
