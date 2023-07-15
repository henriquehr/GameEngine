
#include "engine.h"

void Engine::init() {
    initWindow();
}

void Engine::run() {
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }
        // check if window is minimized and skip drawing
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            continue;
        }
    }
}

void Engine::cleanup() {
    SDL_DestroyWindow(window);
}

void Engine::initWindow() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

    int width = 1600;
    int height = 900;

    window = SDL_CreateWindow("ASHWGA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    SDL_SetWindowResizable(window, SDL_TRUE);
}
