
#include "engine.h"

Engine::Engine() {
    loadModels();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

Engine::~Engine() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void Engine::loadModels() {
    std::vector<Model::Vertex> vertices{{{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}};

    model = std::make_unique<Model>(device, vertices);
}

void Engine::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Engine::createPipeline() {
    PipelineConfigInfo pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapchain.getWidth(), swapchain.getHeight());
    pipelineConfig.renderPass = swapchain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, pipelineConfig, "../../shaders/compiled/simple.vert.spv",
                                          "../../shaders/compiled/simple.frag.spv");
}

void Engine::createCommandBuffers() {
    commandBuffers.resize(swapchain.getImageCount());

    VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = device.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.getDevice(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    for (int i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to being recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassInfo.renderPass = swapchain.getRenderPass();
        renderPassInfo.framebuffer = swapchain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        pipeline->bind(commandBuffers[i]);
        model->bind(commandBuffers[i]);
        model->draw(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }
}

void Engine::drawFrame() {
    uint32_t imageIndex;
    VkResult result = swapchain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    result = swapchain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
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
