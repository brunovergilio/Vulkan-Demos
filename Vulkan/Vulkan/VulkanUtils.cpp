#include "VulkanUtils.h"


void BvDebugPrint(const char * const format, ...)
{
	static thread_local char buf[BV_DEBUG_MSG_LEN]{};
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, BV_DEBUG_MSG_LEN, format, args);
	va_end(args);
	OutputDebugString(buf);
	OutputDebugString("\n");
};


const char * const BvVkGetErrorString(const VkResult result)
{
	static thread_local char s_VulkanError[BV_VULKAN_APP_NAME_MAX_LEN]{};
	switch (result)
	{
	case VK_SUCCESS:						strcpy_s(s_VulkanError, "VK_SUCCESS"); break;
	case VK_NOT_READY:						strcpy_s(s_VulkanError, "VK_NOT_READY"); break;
	case VK_TIMEOUT:						strcpy_s(s_VulkanError, "VK_TIMEOUT"); break;
	case VK_EVENT_SET:						strcpy_s(s_VulkanError, "VK_EVENT_SET"); break;
	case VK_EVENT_RESET:					strcpy_s(s_VulkanError, "VK_EVENT_RESET"); break;
	case VK_INCOMPLETE:						strcpy_s(s_VulkanError, "VK_INCOMPLETE"); break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:		strcpy_s(s_VulkanError, "VK_ERROR_OUT_OF_HOST_MEMORY"); break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:		strcpy_s(s_VulkanError, "VK_ERROR_OUT_OF_DEVICE_MEMORY"); break;
	case VK_ERROR_INITIALIZATION_FAILED:	strcpy_s(s_VulkanError, "VK_ERROR_INITIALIZATION_FAILED"); break;
	case VK_ERROR_DEVICE_LOST:				strcpy_s(s_VulkanError, "VK_ERROR_DEVICE_LOST"); break;
	case VK_ERROR_MEMORY_MAP_FAILED:		strcpy_s(s_VulkanError, "VK_ERROR_MEMORY_MAP_FAILED"); break;
	case VK_ERROR_LAYER_NOT_PRESENT:		strcpy_s(s_VulkanError, "VK_ERROR_LAYER_NOT_PRESENT"); break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:	strcpy_s(s_VulkanError, "VK_ERROR_EXTENSION_NOT_PRESENT"); break;
	case VK_ERROR_FEATURE_NOT_PRESENT:		strcpy_s(s_VulkanError, "VK_ERROR_FEATURE_NOT_PRESENT"); break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:		strcpy_s(s_VulkanError, "VK_ERROR_INCOMPATIBLE_DRIVER"); break;
	case VK_ERROR_TOO_MANY_OBJECTS:			strcpy_s(s_VulkanError, "VK_ERROR_TOO_MANY_OBJECTS"); break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:		strcpy_s(s_VulkanError, "VK_ERROR_FORMAT_NOT_SUPPORTED"); break;
	case VK_ERROR_FRAGMENTED_POOL:			strcpy_s(s_VulkanError, "VK_ERROR_FRAGMENTED_POOL"); break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:		strcpy_s(s_VulkanError, "VK_ERROR_OUT_OF_POOL_MEMORY"); break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:	strcpy_s(s_VulkanError, "VK_ERROR_INVALID_EXTERNAL_HANDLE"); break;
	case VK_ERROR_SURFACE_LOST_KHR:			strcpy_s(s_VulkanError, "VK_ERROR_SURFACE_LOST_KHR"); break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:	strcpy_s(s_VulkanError, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"); break;
	case VK_SUBOPTIMAL_KHR:					strcpy_s(s_VulkanError, "VK_SUBOPTIMAL_KHR"); break;
	case VK_ERROR_OUT_OF_DATE_KHR:			strcpy_s(s_VulkanError, "VK_ERROR_OUT_OF_DATE_KHR"); break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: strcpy_s(s_VulkanError, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"); break;
	case VK_ERROR_VALIDATION_FAILED_EXT:	strcpy_s(s_VulkanError, "VK_ERROR_VALIDATION_FAILED_EXT"); break;
	case VK_ERROR_INVALID_SHADER_NV:		strcpy_s(s_VulkanError, "VK_ERROR_INVALID_SHADER_NV"); break;
	case VK_ERROR_FRAGMENTATION_EXT:		strcpy_s(s_VulkanError, "VK_ERROR_FRAGMENTATION_EXT"); break;
	case VK_ERROR_NOT_PERMITTED_EXT:		strcpy_s(s_VulkanError, "VK_ERROR_NOT_PERMITTED_EXT"); break;
	case VK_RESULT_RANGE_SIZE:				strcpy_s(s_VulkanError, "VK_RESULT_RANGE_SIZE"); break;
	case VK_RESULT_MAX_ENUM:				strcpy_s(s_VulkanError, "VK_RESULT_MAX_ENUM"); break;
	default:								strcpy_s(s_VulkanError, ""); break;
	}

	return s_VulkanError;
}