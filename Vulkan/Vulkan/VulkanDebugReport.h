#pragma once


#include "VulkanInstance.h"


class VulkanDebugReport
{
public:
	VulkanDebugReport(const VulkanInstance & instance);
	~VulkanDebugReport();

	VkResult Create(void * const pData = nullptr, const VkFlags flags =
		VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT);
	void Destroy();

private:
	static VkBool32 VKAPI_PTR DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
		int32_t messageCode, const char * pLayerPrefix, const char * pMessage, void * pUserData);

private:
	const VulkanInstance & m_Instance;
	VkDebugReportCallbackEXT m_DebugReport;
};