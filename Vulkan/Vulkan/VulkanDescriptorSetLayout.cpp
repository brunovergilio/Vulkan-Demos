#include "VulkanDescriptorSetLayout.h"


VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	Destroy();
}


VkResult VulkanDescriptorSetLayout::Create(const VkDescriptorSetLayoutCreateInfo & createInfo)
{
	return vkCreateDescriptorSetLayout(m_Device, &createInfo, nullptr, &m_SetLayout);
}


void VulkanDescriptorSetLayout::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_SetLayout, nullptr, DescriptorSetLayout);
}