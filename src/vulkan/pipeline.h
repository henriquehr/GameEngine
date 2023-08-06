#pragma once

#include "../model.h"
#include "../utils.h"
#include "device.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct PipelineConfigInfo {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    std::vector<VkDynamicState> dynamicStateEnables{};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline {
  public:
    Pipeline(Device &device, const PipelineConfigInfo &pipelineConfigInfo, const std::string &vertFilePath,
             const std::string &fragFilePath);
    ~Pipeline();

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

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
