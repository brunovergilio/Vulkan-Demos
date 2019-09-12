#include "VulkanDebugReport.h"
#include <sstream>


static PFN_vkCreateDebugReportCallbackEXT s_pFnCreateDebugReportCallbackEXT = nullptr;
static PFN_vkDestroyDebugReportCallbackEXT s_pFnDestroyDebugReportCallbackEXT = nullptr;


VulkanDebugReport::VulkanDebugReport(const VulkanInstance & instance)
	: m_Instance(instance)
{
}


VulkanDebugReport::~VulkanDebugReport()
{
	Destroy();
}


VkResult VulkanDebugReport::Create(void * const pData, const VkFlags flags)
{
	VkDebugReportCallbackCreateInfoEXT debugInfo{};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugInfo.pUserData = pData;
	debugInfo.pfnCallback = DebugReportCallback;
	debugInfo.flags = flags;

	s_pFnCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
	s_pFnDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");

	return s_pFnCreateDebugReportCallbackEXT(m_Instance, &debugInfo, nullptr, &m_DebugReport);
}


void VulkanDebugReport::Destroy()
{
	if (m_DebugReport)
	{
		s_pFnDestroyDebugReportCallbackEXT(m_Instance, m_DebugReport, nullptr);
		m_DebugReport = VK_NULL_HANDLE;
	}
}


VkBool32 VulkanDebugReport::DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
	int32_t messageCode, const char * pLayerPrefix, const char * pMessage, void * pUserData)
{
	UNREFERENCED_PARAMETER(flags);
	UNREFERENCED_PARAMETER(objectType);
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(location);
	UNREFERENCED_PARAMETER(messageCode);
	UNREFERENCED_PARAMETER(pLayerPrefix);
	UNREFERENCED_PARAMETER(pMessage);
	UNREFERENCED_PARAMETER(pUserData);

	std::string prefix("");

	// Error that may result in undefined behaviour
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		prefix += "ERROR:";
	};
	// Warnings may hint at unexpected / non-spec API usage
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		prefix += "WARNING:";
	};
	// May indicate sub-optimal usage of the API
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		prefix += "PERFORMANCE:";
	};
	// Informal messages that may become handy during debugging
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		prefix += "INFO:";
	}
	// Diagnostic info from the Vulkan loader and layers
	// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		prefix += "DEBUG:";
	}

	// Display message to default output (console/logcat)
	std::stringstream debugMessage;
	debugMessage << prefix << " [" << pLayerPrefix << "] Code " << messageCode << " : " << pMessage;

	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		MessageBox(nullptr, debugMessage.str().c_str(), "Error", MB_OK);
	}
	else
	{
		OutputDebugString(debugMessage.str().c_str());
	}

	return VK_FALSE;
}