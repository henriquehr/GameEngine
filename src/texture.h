#pragma once

#include "vulkan/device.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>

#include <string>

class Texture {
  public:
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    Texture(Device &device, const std::string &path);
    Texture(Device &device);

    ~Texture();

    VkImage getTextureImage() {
        return textureImage;
    }
    VkDeviceMemory getTextureImageMemory() {
        return textureImageMemory;
    }
    VkImageView getTextureImageView() {
        return textureImageView;
    }
    VkSampler getTextureSampler() {
        return textureSampler;
    }
    VkDescriptorImageInfo getDescriptorImageInfo() {
        return descriptorImageInfo;
    }

  private:
    Device &device;

    uint32_t mipLevels{};
    VkImage textureImage{};
    VkDeviceMemory textureImageMemory{};
    VkImageView textureImageView{};
    VkSampler textureSampler{};
    VkDescriptorImageInfo descriptorImageInfo{};

    void createTextureImage(const std::string &path);

    void createDefaultTextureImage();

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createTextureImageView();

    void createTextureSampler();

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void buildDescriptorImageInfo() {
        descriptorImageInfo = VkDescriptorImageInfo{
                textureSampler,
                textureImageView,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
    }
};
