#pragma once


#include "VulkanDevice.h"


class VulkanFence
{
public:
	explicit VulkanFence(const VulkanDevice & device);
	~VulkanFence();

	VkResult Create(const VkFenceCreateFlags flags = 0);
	void Destroy();

	VkResult Wait(const uint64_t timeout = UINT64_MAX);
	VkResult Reset();

	inline const VkFence Get() const { return m_Fence; }
	inline operator VkFence() const { return m_Fence; }

private:
	const VulkanDevice & m_Device;
	VkFence m_Fence = VK_NULL_HANDLE;
};