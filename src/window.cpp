
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

bool Window::sdlEvents(SDL_Event e, FirstPersonMovementController &cameraController) {
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) {
            return true;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return true;
            }
        } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED &&
                   e.window.windowID == SDL_GetWindowID(getSDLWindow())) {
            setFramebufferResized();
            setWidth(e.window.data1);
            setHeight(e.window.data2);
        } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED &&
                   e.window.windowID == SDL_GetWindowID(getSDLWindow())) {
            setFocused(true);
        } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_LOST &&
                   e.window.windowID == SDL_GetWindowID(getSDLWindow())) {
            setFocused(false);
        } else if (e.type == SDL_MOUSEMOTION && isFocused()) {
            cameraController.processMouseMovement(e.motion.yrel, e.motion.xrel);
        }
    }
    return false;
}