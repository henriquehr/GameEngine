
#include "point_light_system.h"

struct PointLightPushConstants {
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
};

PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, pipelineConfig, "../../shaders/compiled/point_light.vert.spv",
                                          "../../shaders/compiled/point_light.frag.spv");
}

void PointLightSystem::update(FrameInfo frameInfo, GlobalUbo &ubo) {
    int lightIndex = 0;
    for (auto &kv: frameInfo.gameObjects) {
        GameObject &obj = kv.second;
        if (obj.pointLight == nullptr) {
            continue;
        }

        assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

        obj.transform.translation = glm::vec3(glm::vec4(obj.transform.translation, 1.0f));

        ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
        ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
        lightIndex += 1;
    }
    ubo.activeLights = lightIndex;
}

void PointLightSystem::render(FrameInfo frameInfo) {
    pipeline->bind(frameInfo.commandBuffer);

    glm::mat4 projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet,
                            0, nullptr);

    // very inefficient
    for (auto &kv: frameInfo.gameObjects) {
        GameObject &obj = kv.second;
        if (obj.pointLight == nullptr) {
            continue;
        }
        PointLightPushConstants push{};
        push.position = glm::vec4(obj.transform.translation, 1.0f);
        push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
        push.radius = obj.transform.scale.x;

        vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PointLightPushConstants), &push);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}
