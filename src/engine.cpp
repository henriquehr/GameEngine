
#include "engine.h"

Engine::Engine() {
    globalPool = DescriptorPool::Builder(device)
                         .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT + 500)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT + 500)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT + 500)
                         .build();
    loadGameObjects();
}

Engine::~Engine() {}

void Engine::loadGameObjects() {
    std::shared_ptr<Model> model = Model::createModelFromFile(device, "../../models/smooth_vase.obj");
    GameObject vase = GameObject::createGameObject();
    vase.model = model;
    vase.transform.translation = {-0.5f, 0.5f, 0.0f};
    vase.transform.scale = glm::vec3(1.0f, 0.5f, 1.0f);
    gameObjects.emplace(vase.getId(), std::move(vase));

    model = Model::createModelFromFile(device, "../../models/viking_room.obj");
    model->setTexture(std::make_unique<Texture>(device, "../../textures/viking_room.png"));
    GameObject viking_room = GameObject::createGameObject();
    viking_room.model = model;
    viking_room.transform.translation = {0.5f, 0.3f, 0.0f};
    viking_room.transform.rotation = {90.0f, 0.0f, -180.0f};
    viking_room.transform.scale = glm::vec3(0.2f);
    gameObjects.emplace(viking_room.getId(), std::move(viking_room));

    model = Model::createModelFromFile(device, "../../models/lost_empire.obj");
    model->setTexture(std::make_unique<Texture>(device, "../../textures/lost_empire-RGBA.png"));
    GameObject lost_empire = GameObject::createGameObject();
    lost_empire.model = model;
    lost_empire.transform.translation = {0.0f, 16.0f, 0.0f};
    lost_empire.transform.rotation.x = 180.0f;
    lost_empire.transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    gameObjects.emplace(lost_empire.getId(), std::move(lost_empire));

    model = Model::createModelFromFile(device, "../../models/pirate.obj");
    GameObject pirate = GameObject::createGameObject();
    pirate.model = model;
    pirate.transform.translation = {-1.5f, 0.5f, 0.0f};
    pirate.transform.rotation.x = 180.0f;
    pirate.transform.scale = glm::vec3(0.5f);
    gameObjects.emplace(pirate.getId(), std::move(pirate));

    model = Model::createModelFromFile(device, "../../models/quad.obj");
    GameObject floor = GameObject::createGameObject();
    floor.model = model;
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
    gameObjects.emplace(floor.getId(), std::move(floor));

    GameObject pointLight = GameObject::makePointLight(1.8f);
    pointLight.color = {1.0f, 0.0f, 0.0f};
    pointLight.transform.translation = glm::vec3(-1.5f, -1.6f, -1.0f);
    gameObjects.emplace(pointLight.getId(), std::move(pointLight));
}

void Engine::run() {
    // buffer for each frame (2)
    std::vector<std::shared_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto &uboBuffer: uboBuffers) {
        uboBuffer = std::make_shared<Buffer>(device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffer->map();
    }

    std::vector<VkDescriptorImageInfo> descriptorImageInfos;
    int imageIndex = 0;
    for (std::pair<const GameObject::id_t, GameObject> &kv: gameObjects) {
        if (kv.second.model == nullptr) {
            continue;
        }
        descriptorImageInfos.push_back(kv.second.model->getTexture()->getDescriptorImageInfo());
        kv.second.textureIndex = imageIndex++;
    }

    // bind 0 = buffer, bind 1 = all textures (count = imageIndex)
    std::unique_ptr<DescriptorSetLayout> globalSetLayout =
            DescriptorSetLayout::Builder(device)
                    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                    .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, imageIndex)
                    .build();

    // descriptor set for each frame (2) binding 0 = buffer, binding 1 = all textures
    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, descriptorImageInfos.data())
                .build(globalDescriptorSets[i]);
    }

    TextureRenderSystem textureRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    PointLightSystem pointLightSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    ImguiSystem imguiSystem{device, window, renderer};
    Camera camera{};
    FirstPersonMovementController cameraController{glm::vec3(0.0f, 0.0f, -2.5f)};

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
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED &&
                       e.window.windowID == SDL_GetWindowID(window.getSDLWindow())) {
                window.setFocused(true);
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_LOST &&
                       e.window.windowID == SDL_GetWindowID(window.getSDLWindow())) {
                window.setFocused(false);
            } else if (e.type == SDL_MOUSEMOTION && window.isFocused()) {
                cameraController.processMouseMovement(e.motion.yrel, e.motion.xrel);
            }
        }
        // Check if window is minimized and skip drawing
        if (SDL_GetWindowFlags(window.getSDLWindow()) & SDL_WINDOW_MINIMIZED) {
            continue;
        }

        cameraController.processKeyPress(deltaTime);
        camera.setView(cameraController.position, cameraController.front, cameraController.up);
        camera.setPerspectiveProjection(60.0f, renderer.getAspectRatio(), 0.1f, 1000.0f);

        imguiSystem.preRender(&window, camera, cameraController, startupTime);

        if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();
            //render
            renderer.beginSwapChainRenderPass(commandBuffer);
            textureRenderSystem.renderGameObjects(frameInfo);
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
