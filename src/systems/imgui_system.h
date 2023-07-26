#pragma once

#include "../camera.h"
#include "../game_object.h"
#include "../window.h"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

class ImguiSystem {
  public:
    ImguiSystem();
    ~ImguiSystem();

    ImguiSystem(const ImguiSystem &) = delete;
    ImguiSystem &operator=(const ImguiSystem &) = delete;

    void uploadFonts(VkCommandBuffer commandBuffer) const;

    void preRender(Window *window, const Camera &camera, const GameObject &viewerObject);

    void render(VkCommandBuffer commandBuffer);
};
