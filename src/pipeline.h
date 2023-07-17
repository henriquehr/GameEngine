#pragma once

#include "device.h"
#include "model.h"

#include <string>
#include <vector>

struct PipelineConfigInfo {
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline {
  public:
    Pipeline(Device &device, const PipelineConfigInfo &pipelineConfigInfo, const std::string &vertFilePath,
             const std::string &fragFilePath);
    ~Pipeline();

    void bind(VkCommandBuffer commandBuffer);

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

  private:
    Device &device;

    VkPipeline graphicsPipeline = nullptr;
    VkShaderModule vertShaderModule = nullptr;
    VkShaderModule fragShaderModule = nullptr;

    static std::vector<char> readFile(const std::string &filePath);

    void createGraphicsPipeline(const PipelineConfigInfo &pipelineConfigInfo, const std::string &vertFilePath,
                                const std::string &fragFilePath);

    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
};
