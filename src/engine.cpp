
#include "engine.h"

struct GlobalUbo {
    glm::mat4 projectionView{1.0f};
    glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f};// w is intensity
    glm::vec3 lightPosition{-1.0f};
    alignas(16) glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};// w is intensity
};

Engine::Engine() {
    globalPool = DescriptorPool::Builder(device)
                         .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
    loadGameObjects();
}

Engine::~Engine() {}

void Engine::loadGameObjects() {
    std::shared_ptr<Model> model = Model::createModelFromFile(device, "../../models/smooth_vase.obj");
    GameObject vase = GameObject::createGameObject();
    vase.model = model;
    vase.transform.translation = {-0.5f, 0.5f, 0.0f};
    vase.transform.scale = glm::vec3(3.0f);
    gameObjects.emplace(vase.getId(), std::move(vase));

    model = Model::createModelFromFile(device, "../../models/colored_cube.obj");
    GameObject cube = GameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {0.5f, 0.0f, 0.0f};
    cube.transform.scale = glm::vec3(0.5f);
    gameObjects.emplace(cube.getId(), std::move(cube));

    model = Model::createModelFromFile(device, "../../models/pirate.obj");
    GameObject pirate = GameObject::createGameObject();
    pirate.model = model;
    pirate.transform.translation = {-1.5f, 0.5f, 0.0f};
    pirate.transform.rotation.x = glm::radians(180.0f);
    pirate.transform.scale = glm::vec3(1.0f);
    gameObjects.emplace(pirate.getId(), std::move(pirate));

    model = Model::createModelFromFile(device, "../../models/quad.obj");
    GameObject floor = GameObject::createGameObject();
    floor.model = model;
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
    gameObjects.emplace(floor.getId(), std::move(floor));
}

void Engine::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    std::unique_ptr<DescriptorSetLayout> globalSetLayout =
            DescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    Camera camera{};
    GameObject viewerObject = GameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5;
    KeyboardMovementController cameraController{};

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

        cameraController.moveInPlaneXZ(deltaTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 1000.0f);

        if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
            // update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();
            //render
            renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo);
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
    bool oneSecond = (std::chrono::duration<double, std::milli>(newTime - lastUpdateTime).count()) >= 1000.0;
    frameCount++;
    currentTime = newTime;

    if (oneSecond) {
        std::stringstream title;
        title << "FPS:" << std::to_string(frameCount);
        title << "; CPU Time:" << std::format("{:.2f}", (deltaTime * 1000.0)) << "ms";

        std::cout << title.str() << std::endl;
        SDL_SetWindowTitle(window.getSDLWindow(), title.str().c_str());

        frameCount = 0;
        lastUpdateTime = newTime;
    }
}
