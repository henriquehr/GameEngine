
#include "engine.h"

Engine::Engine() {
    loadGameObjects();
}

Engine::~Engine() {}

void sierpinski(std::vector<Model::Vertex> &vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top) {
    if (depth <= 0) {
        vertices.push_back({top, {1.0f, 0.0f, 0.0f}});
        vertices.push_back({right, {0.0f, 1.0f, 0.0f}});
        vertices.push_back({left, {0.0f, 0.0f, 1.0f}});
    } else {
        auto leftTop = 0.5f * (left + top);
        auto rightTop = 0.5f * (right + top);
        auto leftRight = 0.5f * (left + right);
        sierpinski(vertices, depth - 1, left, leftRight, leftTop);
        sierpinski(vertices, depth - 1, leftRight, right, rightTop);
        sierpinski(vertices, depth - 1, leftTop, rightTop, top);
    }
}

void Engine::sierpinskiTriangle() {
    std::vector<Model::Vertex> vertices{};
    sierpinski(vertices, 6, {-0.9f, 0.9f}, {0.9f, 0.9f}, {0.0f, -0.9f});
    std::shared_ptr<Model> model = std::make_shared<Model>(device, vertices);

    GameObject triangle = GameObject::createGameObject();
    triangle.model = model;
    triangle.color = {0.1f, 0.8f, 0.1f};
    triangle.transform2D.translation.x = 0.2f;
    triangle.transform2D.scale = {2.0f, 0.5f};
    triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

    gameObjects.push_back(std::move(triangle));
}

void Engine::loadGameObjects() {
    //    sierpinskiTriangle();
    std::vector<Model::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    std::shared_ptr<Model> model = std::make_shared<Model>(device, vertices);

    GameObject triangle = GameObject::createGameObject();
    triangle.model = model;
    triangle.color = {0.1f, 0.8f, 0.1f};
    triangle.transform2D.translation.x = 0.2f;
    triangle.transform2D.scale = {2.0f, 0.5f};
    triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

    gameObjects.push_back(std::move(triangle));
}

void Engine::run() {
    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};

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
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED &&
                       e.window.windowID == SDL_GetWindowID(window.getSDLWindow())) {
                window.setFramebufferResized();
                window.setWidth(e.window.data1);
                window.setHeight(e.window.data2);
            }
        }
        // Check if window is minimized and skip drawing
        if (SDL_GetWindowFlags(window.getSDLWindow()) & SDL_WINDOW_MINIMIZED) {
            continue;
        }

        if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }

        fps();
    }

    vkDeviceWaitIdle(device.getDevice());
}

void Engine::fps() {
    end = clock::now();
    std::chrono::duration<float> elapsedTime = end - start;
    bool oneSecond = (std::chrono::duration<double, std::milli>(end - lastUpdateTime).count()) >= 1000.0;
    start = clock::now();
    frameCount++;
    if (oneSecond) {
        std::stringstream title;
        title << "FPS:" << std::to_string(frameCount);
        title << "; CPU Time:" << std::format("{:.2f}", (elapsedTime.count() * 1000.0)) << "ms";

        std::cout << title.str() << std::endl;
        SDL_SetWindowTitle(window.getSDLWindow(), title.str().c_str());

        frameCount = 0;
        lastUpdateTime = start;
    }
}
