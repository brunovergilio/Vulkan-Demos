#pragma once


#include "VulkanDevice.h"


class VulkanPipelineCache
{
public:
	explicit VulkanPipelineCache(const VulkanDevice & device);
	~VulkanPipelineCache();

	VkResult Create(size_t initialDataSize = 0, const void * const pInitialData = nullptr);
	void Destroy();

	inline const VkPipelineCache Get() const { return m_Cache; }
	inline operator VkPipelineCache() const { return m_Cache; }

private:
	const VulkanDevice & m_Device;
	VkPipelineCache m_Cache = VK_NULL_HANDLE;
};