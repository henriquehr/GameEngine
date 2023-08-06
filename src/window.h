#pragma once

#include "first_person_movement_controller.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <imgui_impl_sdl2.h>
#include <vulkan/vulkan.h>

#include "stdexcept"

class Window {
  public:
    Window(int width, int height);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    SDL_Window *getSDLWindow() {
        return window;
    }
    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    };
    bool wasWindowResized() {
        return framebufferResized;
    }
    void resetWindowResizedFlag() {
        framebufferResized = false;
    }
    void setWidth(int width) {
        this->width = width;
    };
    void setHeight(int height) {
        this->height = height;
    };
    void setFramebufferResized() {
        framebufferResized = true;
    }
    void setFocused(bool focused) {
        this->focused = focused;
    }
    bool isFocused() {
        return focused;
    }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    bool sdlEvents(SDL_Event e, FirstPersonMovementController &cameraController);

  private:
    SDL_Window *window{};

    int width{};
    int height{};
    bool framebufferResized = false;
    bool focused = false;

    void initWindow();
};
