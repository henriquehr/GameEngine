
#include "texture_render_system.h"

struct TexturePushConstantData {
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 normalMatrix{1.0f};
    int textureIndex = 0;
};

TextureRenderSystem::TextureRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device) {
    std::cout << "TexturePushConstantData size: " << sizeof(TexturePushConstantData) << std::endl;
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

TextureRenderSystem::~TextureRenderSystem() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void TextureRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(TexturePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    VK_CHECK(vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout));
}

void TextureRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, pipelineConfig, "../../shaders/compiled/texture.vert.spv",
                                          "../../shaders/compiled/texture.frag.spv");
}

void TextureRenderSystem::renderGameObjects(FrameInfo frameInfo) {
    pipeline->bind(frameInfo.commandBuffer);

    glm::mat4 projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet,
                            0, nullptr);

    for (std::pair<const GameObject::id_t, GameObject> &kv: frameInfo.gameObjects) {
        GameObject &obj = kv.second;
        if (obj.model == nullptr) {
            continue;
        }

        TexturePushConstantData push{};
        push.modelMatrix = obj.transform.modelMatrix();
        push.normalMatrix = obj.transform.normalMatrix();
        push.textureIndex = obj.textureIndex;

        vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(TexturePushConstantData), &push);

        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }
}
