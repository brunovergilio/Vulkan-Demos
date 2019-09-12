#include "VulkanFence.h"


VulkanFence::VulkanFence(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanFence::~VulkanFence()
{
	Destroy();
}


VkResult VulkanFence::Create(const VkFenceCreateFlags flags)
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = flags;

	return vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Fence);
}


void VulkanFence::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Fence, nullptr, Fence);
}


VkResult VulkanFence::Wait(const uint64_t timeout)
{
	return vkWaitForFences(m_Device, 1, &m_Fence, VK_TRUE, timeout);
}


VkResult VulkanFence::Reset()
{
	return vkResetFences(m_Device, 1, &m_Fence);
}