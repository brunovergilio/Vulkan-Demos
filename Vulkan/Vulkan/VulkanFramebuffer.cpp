#include "VulkanFramebuffer.h"


VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanFramebuffer::~VulkanFramebuffer()
{
	Destroy();
}


VkResult VulkanFramebuffer::Create(const VkFramebufferCreateInfo & frameBufferCreateInfo)
{
	return vkCreateFramebuffer(m_Device, &frameBufferCreateInfo, nullptr, &m_FrameBuffer);
}


void VulkanFramebuffer::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_FrameBuffer, nullptr, Framebuffer);
}