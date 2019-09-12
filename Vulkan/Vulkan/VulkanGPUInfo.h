#pragma once


#include "VulkanUtils.h"


struct VulkanGPUInfo
{
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties m_DeviceProperties{};
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	std::vector<std::string> m_SupportedExtensions;
};
typedef std::vector<VulkanGPUInfo> VulkanGPUInfoList;