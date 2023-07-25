#pragma once

#include "camera.h"
#include "game_object.h"

#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer{};
    Camera &camera;
    VkDescriptorSet globalDescriptorSet{};
    GameObject::Map &gameObjects;
};

struct PointLight {
    glm::vec4 position{};// ignore w
    glm::vec4 color{};   // w is intensity
};

struct GlobalUbo {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 inverseView{1.0f};
    glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.002f};// w is intensity
    PointLight potinLights[MAX_LIGHTS];
    int activeLights;
};
