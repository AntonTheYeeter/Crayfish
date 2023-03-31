#include "vulkan_graphics_pipeline.h"

void createGraphicsPipeline(VulkanContext* context)
{
    VkGraphicsPipelineCreateInfo gpInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    gpInfo.stageCount;
    gpInfo.pStages;
    gpInfo.pVertexInputState;
    gpInfo.pInputAssemblyState;
    gpInfo.pTessellationState;
    gpInfo.pViewportState;
    gpInfo.pRasterizationState;
    gpInfo.pMultisampleState;
    gpInfo.pDepthStencilState;
    gpInfo.pColorBlendState;
    gpInfo.pDynamicState;
    gpInfo.layout;
    gpInfo.renderPass;
    gpInfo.subpass;
    gpInfo.basePipelineHandle;
    gpInfo.basePipelineIndex;
}