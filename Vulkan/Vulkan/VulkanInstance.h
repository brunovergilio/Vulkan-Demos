#pragma once


#include "VulkanGPUInfo.h"


class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	VkResult Create(const char * const pAppName, const VkBool32 debugSupport);
	void Destroy();

	VkResult EnumerateGPUs(VulkanGPUInfoList & gpuInfoList);

	inline const VkInstance Get() const { return m_Instance; }
	inline operator VkInstance() const { return m_Instance; }

private:
	VkInstance m_Instance = VK_NULL_HANDLE;
};