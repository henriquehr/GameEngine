
#include "window.h"

Window::Window() {
    initWindow();
}

Window::~Window() {
    SDL_DestroyWindow(window);
}

void Window::initWindow() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

    int width = 1600;
    int height = 900;

    window = SDL_CreateWindow("ASHWGA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    SDL_SetWindowResizable(window, SDL_TRUE);
}

SDL_Window *Window::getSDLWindow() {
    return window;
}
