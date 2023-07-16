
#include "window.h"

Window::Window(int width, int height) : width(width), height(height) {
    initWindow();
}

Window::~Window() {
    SDL_DestroyWindow(window);
}

void Window::initWindow() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

    window = SDL_CreateWindow("ASHWGA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    SDL_SetWindowResizable(window, SDL_TRUE);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (SDL_Vulkan_CreateSurface(window, instance, surface) == SDL_FALSE) {
        throw std::runtime_error(std::string("Failed to create surface, SDL error:") + SDL_GetError());
    }
}
