
#include "engine.h"

struct SimplePushConstantData {
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

Engine::Engine() {
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
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
    model = std::make_unique<Model>(device, vertices);
}

void Engine::loadModels() {
    //    sierpinskiTriangle();
    std::vector<Model::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    model = std::make_unique<Model>(device, vertices);
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
    assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, pipelineConfig, "../../shaders/compiled/simple.vert.spv",
                                          "../../shaders/compiled/simple.frag.spv");
}

void Engine::recreateSwapChain() {
    VkExtent2D extent = window.getExtent();
    SDL_Event e;
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        SDL_WaitEvent(&e);
    }
    vkDeviceWaitIdle(device.getDevice());

    if (swapChain == nullptr) {
        swapChain = std::make_unique<SwapChain>(device, extent);
    } else {
        swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
        if (swapChain->getImageCount() != commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
    }
    createPipeline();
}

void Engine::createCommandBuffers() {
    commandBuffers.resize(swapChain->getImageCount());

    VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = device.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.getDevice(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void Engine::freeCommandBuffers() {
    vkFreeCommandBuffers(device.getDevice(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
}

void Engine::recordCommandBuffer(int imageIndex) {
    static int frame = 0;
    frame = (frame + 1) % 4000;

    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to being recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    pipeline->bind(commandBuffers[imageIndex]);
    model->bind(commandBuffers[imageIndex]);

    for (int i = 0; i < 4; i++) {
        SimplePushConstantData push{};
        push.offset = {-0.5f + frame * 0.0005f, -0.4f + i * 0.25f};
        push.color = {0.0f, 0.0f, 0.2f + 0.2f * i};

        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        model->draw(commandBuffers[imageIndex]);
    }

    vkCmdEndRenderPass(commandBuffers[imageIndex]);
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }
}

void Engine::drawFrame() {
    uint32_t imageIndex;
    VkResult result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    recordCommandBuffer(imageIndex);
    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image");
    }
}

void Engine::init() {}

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

        drawFrame();

        fps();
    }

    vkDeviceWaitIdle(device.getDevice());
}

void Engine::cleanup() {}

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
