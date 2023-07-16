#pragma once

#include "device.h"

#include "vulkan/vulkan.h"

#include <array>
#include <iostream>
#include <string>
#include <vector>

class Swapchain {
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    Swapchain(Device &deviceRef, VkExtent2D windowExtent);
    ~Swapchain();

    VkFramebuffer getFrameBuffer(int index) {
        return swapChainFramebuffers[index];
    }
    VkRenderPass getRenderPass() {
        return renderPass;
    }
    VkImageView getImageView(int index) {
        return swapChainImageViews[index];
    }
    size_t getImageCount() {
        return swapChainImages.size();
    }
    VkFormat getSwapChainImageFormat() {
        return swapChainImageFormat;
    }
    VkExtent2D getSwapChainExtent() {
        return swapChainExtent;
    }
    uint32_t getWidth() {
        return swapChainExtent.width;
    }
    uint32_t getHeight() {
        return swapChainExtent.height;
    }
    float getExtentAspectRatio() {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }

    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  private:
    Device &device;

    size_t currentFrame = 0;

    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtent{};

    std::vector<VkFramebuffer> swapChainFramebuffers{};
    VkRenderPass renderPass = nullptr;

    std::vector<VkImage> depthImages{};
    std::vector<VkDeviceMemory> depthImageMemories{};
    std::vector<VkImageView> depthImageViews{};
    std::vector<VkImage> swapChainImages{};
    std::vector<VkImageView> swapChainImageViews{};

    VkExtent2D windowExtent{};

    VkSwapchainKHR swapChain = nullptr;

    std::vector<VkSemaphore> imageAvailableSemaphores{};
    std::vector<VkSemaphore> renderFinishedSemaphores{};
    std::vector<VkFence> inFlightFences{};
    std::vector<VkFence> imagesInFlight{};

    void createSwapChain();

    void createImageViews();

    void createDepthResources();

    void createRenderPass();

    void createFramebuffers();

    void createSyncObjects();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};
