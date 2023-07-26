#pragma once

#include "imgui_impl_vulkan.h"

class ImguiSystem {
  public:
    ImguiSystem();
    ~ImguiSystem();

    ImguiSystem(const ImguiSystem &) = delete;
    ImguiSystem &operator=(const ImguiSystem &) = delete;

    void uploadFonts(VkCommandBuffer commandBuffer) const;

    void render(VkCommandBuffer commandBuffer);
};
