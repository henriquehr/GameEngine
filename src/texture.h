#pragma once

#include "device.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>

#include <string>

class Texture {
  public:
    Texture(Device &device, std::string path);

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
    VkDescriptorImageInfo getDescriptorInfo(VkImageView textureImageView, VkSampler textureSampler) {
        return VkDescriptorImageInfo{
                textureSampler,
                textureImageView,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
    }

  private:
    Device &device;

    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    void createTextureImage(std::string path);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createTextureImageView();

    void createTextureSampler();

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
};
