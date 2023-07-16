#pragma once

#include "SDL.h"
#include "SDL_vulkan.h"
#include "vulkan/vulkan.h"

#include "stdexcept"

class Window {
  public:
    Window(int width, int height);
    ~Window();

    SDL_Window *getSDLWindow() {
        return window;
    }
    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    };

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    SDL_Window *window{};

    int width{};
    int height{};

    void initWindow();
};
