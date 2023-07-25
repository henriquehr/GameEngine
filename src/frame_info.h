#pragma once

#include "camera.h"
#include "game_object.h"

#include <vulkan/vulkan.h>

struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer{};
    Camera &camera;
    VkDescriptorSet globalDescriptorSet{};
    GameObject::Map &gameObjects;
};