
#include "imgui_system.h"

ImguiSystem::ImguiSystem() {}

ImguiSystem::~ImguiSystem() {}

void ImguiSystem::uploadFonts(VkCommandBuffer commandBuffer) const {
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
}

void ImguiSystem::preRender(Window *window, const Camera &camera, const GameObject &viewerObject) {
    //imgui new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->getSDLWindow());
    ImGui::NewFrame();
    //imgui commands
    float framerate = ImGui::GetIO().Framerate;
    ImGui::Text("Frametime: %.3fms, FPS: %.1f", 1000.0f / framerate, framerate);
    if (ImGui::CollapsingHeader("CAMERA")) {
        if (ImGui::TreeNode("Position")) {
            if (ImGui::BeginTable("table1", 3)) {
                for (int row = 0; row < 1; row++) {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++) {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("%f", viewerObject.transform.translation[column]);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("View Matrix")) {
            if (ImGui::BeginTable("table1", 4)) {
                for (int row = 0; row < 4; row++) {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++) {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("%f", camera.getView()[row][column]);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Projection Matrix")) {
            if (ImGui::BeginTable("table1", 4)) {
                for (int row = 0; row < 4; row++) {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++) {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("%f", camera.getProjection()[row][column]);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
    }
    ImGui::Render();
}

void ImguiSystem::render(VkCommandBuffer commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
