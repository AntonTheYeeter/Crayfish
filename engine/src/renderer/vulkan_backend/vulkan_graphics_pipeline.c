#include "vulkan_graphics_pipeline.h"

#include "core/filesystem.h"
#include "renderer/renderer.h"

static VkShaderModule createShaderModule(VkDevice device, VkAllocationCallbacks* allocator, const char* shaderFilename)
{
    u64 shaderFilesize = 0;
    const char* shaderSource = fileReadAll(shaderFilename, &shaderFilesize);

    VkShaderModuleCreateInfo smInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    smInfo.codeSize = shaderFilesize;
    smInfo.pCode = (const u32*)shaderSource;

    VkShaderModule module;
    VK_CHECK(vkCreateShaderModule(device, &smInfo, allocator, &module));

    return module;
}

void createGraphicsPipeline(VulkanContext* context)
{
    VkShaderModule vertModule = createShaderModule(context->device, context->allocator, "../engine/res/shaders/vert.spv");
    VkShaderModule fragModule = createShaderModule(context->device, context->allocator, "../engine/res/shaders/frag.spv");

    VkPipelineShaderStageCreateInfo vertStage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertModule;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragModule;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStage, fragStage};

    VkGraphicsPipelineCreateInfo gpInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    gpInfo.stageCount = 2;
    gpInfo.pStages = shaderStages;

    VkVertexInputBindingDescription bindingDescription = {0};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription positionAttribute = {0};
    positionAttribute.location = 0;
    positionAttribute.binding = 0;
    positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttribute.offset = offsetof(Vertex, position);

    VkVertexInputAttributeDescription colorAttribute = {0};
    colorAttribute.location = 1;
    colorAttribute.binding = 0;
    colorAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    colorAttribute.offset = offsetof(Vertex, color);

    VkVertexInputAttributeDescription texCoordAttribute = {0};
    texCoordAttribute.location = 2;
    texCoordAttribute.binding = 0;
    texCoordAttribute.format = VK_FORMAT_R32G32_SFLOAT;
    texCoordAttribute.offset = offsetof(Vertex, texCoord);

    VkVertexInputAttributeDescription normalAttribute = {0};
    normalAttribute.location = 3;
    normalAttribute.binding = 0;
    normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttribute.offset = offsetof(Vertex, normal);

    VkVertexInputAttributeDescription attributes[] = {positionAttribute, colorAttribute, texCoordAttribute, normalAttribute};

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDescription;
    vertexInput.vertexAttributeDescriptionCount = ARRAY_SIZE(attributes);
    vertexInput.pVertexAttributeDescriptions = attributes;
    
    gpInfo.pVertexInputState = &vertexInput;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    gpInfo.pInputAssemblyState = &inputAssembly;

    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    gpInfo.pViewportState = &viewportState;

    VkPipelineRasterizationStateCreateInfo rasterization = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization.depthClampEnable = VK_FALSE;
    rasterization.rasterizerDiscardEnable = VK_FALSE;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_NONE;
    rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization.depthBiasEnable = VK_FALSE;
    rasterization.lineWidth = 1.0f;

    gpInfo.pRasterizationState = &rasterization;

    VkPipelineMultisampleStateCreateInfo multisampling = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    gpInfo.pMultisampleState = &multisampling;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlending = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    gpInfo.pColorBlendState = &colorBlending;

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicStateInfo.dynamicStateCount = ARRAY_SIZE(dynamicStates);
    dynamicStateInfo.pDynamicStates = dynamicStates;

    gpInfo.pDynamicState = &dynamicStateInfo;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VK_CHECK(vkCreatePipelineLayout(context->device, &pipelineLayoutInfo, context->allocator, &context->pipelineLayout));

    gpInfo.layout = context->pipelineLayout;
    gpInfo.renderPass = context->renderPass;

    vkCreateGraphicsPipelines(context->device, VK_NULL_HANDLE, 1, &gpInfo, context->allocator, &context->graphicsPipeline);

    vkDestroyShaderModule(context->device, fragModule, context->allocator);
    vkDestroyShaderModule(context->device, vertModule, context->allocator);
}