#pragma once

#include "device.h"
#include "model.h"
#include "swapchain.h"
#include "window.h"

#include <array>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

// swapchain, command buffers, and drawing
class Renderer {
  public:
    Renderer(Window &window, Device &device);
    ~Renderer();

    VkRenderPass getSwapChainRenderPass() const {
        return swapChain->getRenderPass();
    };

    bool isFrameInProgress() const {
        return isFrameStarted;
    };

    VkCommandBuffer getCurrentCommandBuffer() {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();

    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    Window &window;
    Device &device;
    std::unique_ptr<SwapChain> swapChain = nullptr;
    std::vector<VkCommandBuffer> commandBuffers{};

    uint32_t currentImageIndex = 0;
    int currentFrameIndex = 0;
    bool isFrameStarted = false;

    void createCommandBuffers();

    void freeCommandBuffers();

    void recreateSwapChain();
};
