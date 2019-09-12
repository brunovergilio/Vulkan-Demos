#pragma once


#include "VulkanDevice.h"


class VulkanSemaphore
{
public:
	VulkanSemaphore() = delete;
	explicit VulkanSemaphore(const VulkanDevice & device);
	~VulkanSemaphore();

	VkResult Create();
	void Destroy();

	inline const VkSemaphore Get() const { return m_Semaphore; }
	inline operator VkSemaphore() const { return m_Semaphore; }

private:
	const VulkanDevice & m_Device;
	VkSemaphore m_Semaphore = VK_NULL_HANDLE;
};