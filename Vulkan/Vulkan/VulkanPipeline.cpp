#include "VulkanPipeline.h"


VulkanPipeline::VulkanPipeline(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanPipeline::~VulkanPipeline()
{
	Destroy();
}


VkResult VulkanPipeline::Create(const VkGraphicsPipelineCreateInfo & createInfo, const VkPipelineCache pipelineCache)
{
	return vkCreateGraphicsPipelines(m_Device, pipelineCache, 1, &createInfo, nullptr, &m_Pipeline);
}


void VulkanPipeline::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Pipeline, nullptr, Pipeline);
}