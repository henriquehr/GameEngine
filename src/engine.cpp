
#include "engine.h"

struct SimplePushConstantData {
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

Engine::Engine() {
    loadGameObjects();
    createPipelineLayout();
    createPipeline();
}

Engine::~Engine() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

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

void Engine::createPipelineLayout() {

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Engine::createPipeline() {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderer.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, pipelineConfig, "../../shaders/compiled/simple.vert.spv",
                                          "../../shaders/compiled/simple.frag.spv");
}

void Engine::renderGameObjects(VkCommandBuffer commandBuffer) {
    pipeline->bind(commandBuffer);

    for (GameObject &obj: gameObjects) {
        obj.transform2D.rotation = glm::mod(obj.transform2D.rotation + 0.0005f, glm::two_pi<float>());

        SimplePushConstantData push{};
        push.offset = obj.transform2D.translation;
        push.color = obj.color;
        push.transform = obj.transform2D.mat2();

        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
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
            renderGameObjects(commandBuffer);
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
