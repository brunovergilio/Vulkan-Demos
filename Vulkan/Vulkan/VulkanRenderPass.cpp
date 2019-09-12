#include "VulkanRenderPass.h"


VulkanRenderPass::VulkanRenderPass(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanRenderPass::~VulkanRenderPass()
{
	Destroy();
}


VkResult VulkanRenderPass::Create(const VkRenderPassCreateInfo & renderPassCreateInfo)
{
	return vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass);
}


void VulkanRenderPass::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_RenderPass, nullptr, RenderPass);
}