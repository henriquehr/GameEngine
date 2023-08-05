
#include "imgui_system.h"

ImguiSystem::ImguiSystem(Device &device, Window &window, Renderer &renderer) : device(device) {
    init(window, renderer);
}

ImguiSystem::~ImguiSystem() {
    vkDestroyDescriptorPool(device.getDevice(), imguiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

void ImguiSystem::uploadFonts(VkCommandBuffer commandBuffer) const {
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
}

void ImguiSystem::preRender(Window *window, const Camera &camera, FirstPersonMovementController cameraController, float startupTime) {
    //imgui new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->getSDLWindow());
    ImGui::NewFrame();
    //imgui commands
    float framerate = ImGui::GetIO().Framerate;
    ImGui::Text("Frametime: %.3fms, FPS: %.1f, Startup Time: %.3fms", 1000.0f / framerate, framerate, startupTime);
    if (ImGui::CollapsingHeader("CAMERA", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Controller")) {
            if (ImGui::BeginTable("table1", 4)) {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++) {
                    ImGui::TableSetColumnIndex(column);
                    if (column == 0) {
                        ImGui::Text("Positon: ");
                        ImGui::SameLine();
                    }
                    ImGui::Text("%f", cameraController.position[column]);
                }
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++) {
                    ImGui::TableSetColumnIndex(column);
                    if (column == 0) {
                        ImGui::Text("Front: ");
                        ImGui::SameLine();
                    }
                    ImGui::Text("%f", cameraController.front[column]);
                }
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++) {
                    ImGui::TableSetColumnIndex(column);
                    if (column == 0) {
                        ImGui::Text("Up: ");
                        ImGui::SameLine();
                    }
                    ImGui::Text("%f", cameraController.up[column]);
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

void ImguiSystem::init(Window &window, Renderer &renderer) {
    //1: create descriptor pool for IMGUI
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VK_CHECK(vkCreateDescriptorPool(device.getDevice(), &pool_info, nullptr, &imguiPool));

    // 2: initialize imgui library
    //this initializes the core structures of imgui
    ImGui::CreateContext();

    //this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(window.getSDLWindow());

    //this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device.getInstance();
    init_info.PhysicalDevice = device.getPhysicalDevice();
    init_info.Device = device.getDevice();
    init_info.Queue = device.getGraphicsQueue();
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, renderer.getSwapChainRenderPass());

    if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
        uploadFonts(commandBuffer);
        renderer.beginSwapChainRenderPass(commandBuffer);
        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
        //        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}