#pragma once

#include "utils.h"
#include "window.h"

#include "SDL_vulkan.h"
#include "vulkan/vulkan.h"

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    uint32_t graphicsFamily{};
    uint32_t presentFamily{};
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() {
        return graphicsFamilyHasValue && presentFamilyHasValue;
    }
};

class Device {
  public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    VkPhysicalDeviceProperties properties{};

    Device(Window &window);
    ~Device();

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    VkCommandPool getCommandPool() {
        return commandPool;
    }
    VkDevice getDevice() {
        return device;
    }
    VkSurfaceKHR getSurface() {
        return surface;
    }
    VkQueue getGraphicsQueue() {
        return graphicsQueue;
    }
    VkQueue getPresentQueue() {
        return presentQueue;
    }
    VkInstance getInstance() {
        return instance;
    }
    VkPhysicalDevice getPhysicalDevice() {
        return physicalDevice;
    }

    SwapChainSupportDetails getSwapChainSupport();

    QueueFamilyIndices findPhysicalQueueFamilies();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

    void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image,
                             VkDeviceMemory &imageMemory);

  private:
    Window &window;

    VkInstance instance = nullptr;
    VkDebugUtilsMessengerEXT debugMessenger = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    VkCommandPool commandPool = nullptr;
    VkDevice device = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkQueue graphicsQueue = nullptr;
    VkQueue presentQueue = nullptr;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    void createInstance();

    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createCommandPool();

    bool isDeviceSuitable(VkPhysicalDevice device);

    std::vector<const char *> getRequiredExtensions();

    bool checkValidationLayerSupport();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void hasSDLRequiredInstanceExtensions();

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
};
