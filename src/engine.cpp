
#include "engine.h"

Engine::Engine() {
    globalPool = DescriptorPool::Builder(device)
                         .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
    loadGameObjects();
}

Engine::~Engine() {
    vkDestroyDescriptorPool(device.getDevice(), imguiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

void Engine::loadGameObjects() {
    std::shared_ptr<Model> model = Model::createModelFromFile(device, "../../models/smooth_vase.obj");
    GameObject vase = GameObject::createGameObject();
    vase.model = model;
    vase.transform.translation = {-0.5f, 0.5f, 0.0f};
    vase.transform.scale = glm::vec3(1.0f, 0.5f, 1.0f);
    gameObjects.emplace(vase.getId(), std::move(vase));

    model = Model::createModelFromFile(device, "../../models/colored_cube.obj");
    GameObject cube = GameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {0.5f, 0.3f, 0.0f};
    cube.transform.scale = glm::vec3(0.2f);
    gameObjects.emplace(cube.getId(), std::move(cube));

    model = Model::createModelFromFile(device, "../../models/pirate.obj");
    GameObject pirate = GameObject::createGameObject();
    pirate.model = model;
    pirate.transform.translation = {-1.5f, 0.5f, 0.0f};
    pirate.transform.rotation.x = glm::radians(180.0f);
    pirate.transform.scale = glm::vec3(0.5f);
    gameObjects.emplace(pirate.getId(), std::move(pirate));

    model = Model::createModelFromFile(device, "../../models/quad.obj");
    GameObject floor = GameObject::createGameObject();
    floor.model = model;
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
    gameObjects.emplace(floor.getId(), std::move(floor));

    std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f}, {.1f, .1f, 1.f}, {.1f, 1.f, .1f}, {1.f, 1.f, .1f}, {.1f, 1.f, 1.f}, {1.f, 1.f, 1.f}//
    };
    for (int i = 0; i < lightColors.size(); i++) {
        GameObject pointLight = GameObject::makePointLight(0.2f);
        pointLight.color = lightColors[i];
        glm::mat4 rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / lightColors.size(), {0.0f, -1.0f, 0.0f});
        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
        gameObjects.emplace(pointLight.getId(), std::move(pointLight));
    }
}

void Engine::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto &uboBuffer: uboBuffers) {
        uboBuffer = std::make_unique<Buffer>(device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffer->map();
    }

    std::unique_ptr<DescriptorSetLayout> globalSetLayout =
            DescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    PointLightSystem pointLightSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    ImguiSystem imguiSystem{};
    Camera camera{};
    GameObject viewerObject = GameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5;
    FirstPersonMovementController cameraController{};

    init_imgui(imguiSystem);

    float startupTime = std::chrono::duration<float, std::chrono::seconds::period>(clock::now() - currentTime).count();
    std::cout << "Startup time: " << startupTime << " seconds" << std::endl;
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
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
            } else if (e.type == SDL_MOUSEMOTION) {
                cameraController.setPitchYaw(e.motion.yrel, e.motion.xrel);
            }
        }
        // Check if window is minimized and skip drawing
        if (SDL_GetWindowFlags(window.getSDLWindow()) & SDL_WINDOW_MINIMIZED) {
            continue;
        }

        cameraController.move(deltaTime, viewerObject);
        camera.setView(viewerObject.transform.translation, cameraController.getMouseRotation());

        float aspect = renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 1000.0f);

        imguiSystem.preRender(&window, camera, viewerObject, cameraController, startupTime);

        if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();
            //render
            renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo);
            pointLightSystem.render(frameInfo);
            imguiSystem.render(commandBuffer);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }

        fps();
    }

    vkDeviceWaitIdle(device.getDevice());
}

void Engine::fps() {
    std::chrono::time_point newTime = clock::now();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;
    //    bool oneSecond = (std::chrono::duration<double, std::milli>(newTime - lastUpdateTime).count()) >= 1000.0;
    //    frameCount++;

    //    if (oneSecond) {
    //        std::stringstream title;
    //        title << "FPS:" << std::to_string(frameCount);
    //        title << "; CPU Time:" << std::format("{:.2f}", (deltaTime * 1000.0)) << "ms";
    //
    //        //        std::cout << title.str() << std::endl;
    //        SDL_SetWindowTitle(window.getSDLWindow(), title.str().c_str());
    //
    //        frameCount = 0;
    //        lastUpdateTime = newTime;
    //    }
}

void Engine::init_imgui(const ImguiSystem &imguiSystem) {
    //1: create descriptor pool for IMGUI
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VK_CHECK(vkCreateDescriptorPool(device.getDevice(), &pool_info, nullptr, &imguiPool));

    // 2: initialize imgui library
    //this initializes the core structures of imgui
    ImGui::CreateContext();

    //this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(window.getSDLWindow());

    //this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device.getInstance();
    init_info.PhysicalDevice = device.getPhysicalDevice();
    init_info.Device = device.getDevice();
    init_info.Queue = device.getGraphicsQueue();
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, renderer.getSwapChainRenderPass());

    if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
        imguiSystem.uploadFonts(commandBuffer);
        renderer.beginSwapChainRenderPass(commandBuffer);
        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
        //        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}
