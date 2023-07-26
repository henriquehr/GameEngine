
#include "imgui_system.h"

ImguiSystem::ImguiSystem() {}

ImguiSystem::~ImguiSystem() {}

void ImguiSystem::uploadFonts(VkCommandBuffer commandBuffer) const {
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
}

void ImguiSystem::render(VkCommandBuffer commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
