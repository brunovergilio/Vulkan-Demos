#include "VulkanPipelineLayout.h"


VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanPipelineLayout::~VulkanPipelineLayout()
{
	Destroy();
}


VkResult VulkanPipelineLayout::Create(const VkPipelineLayoutCreateInfo & createInfo)
{
	return vkCreatePipelineLayout(m_Device, &createInfo, nullptr, &m_Layout);
}


void VulkanPipelineLayout::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Layout, nullptr, PipelineLayout);
}