#pragma once


//#if defined(_MSC_VER)
#define BV_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR

#define BV_FUNCTION __FUNCTION__

#include <Windows.h>
//#else
// Other platforms
//#endif

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include <functional>


const char * const BvVkGetErrorString(const VkResult result);


#if defined(_DEBUG) || defined(DEBUG)
#define BV_DEBUG
#endif // #if defined(_DEBUG) || defined(DEBUG)

#if defined(BV_DEBUG)
#define BV_ASSERT(cond) assert(cond)
#define BV_ASSERT_MSG(cond, msg) BV_ASSERT((msg && cond))
#define BV_DEBUG_MSG_LEN 2048


//#if defined(BV_WIN32)

#include <intrin.h>

void BvDebugPrint(const char * const format, ...);
inline void BvDebugBreak() { __debugbreak(); }
#define BV_INFO(...) \
{ \
	BvDebugPrint("[BvInfo]: "); \
	BvDebugPrint(__VA_ARGS__); \
}
#define BV_WARNING(...) \
{ \
	static bool skip = false; \
	BvDebugPrint("[BvWarning]: "); \
	BvDebugPrint(__VA_ARGS__); \
	if (!skip) { BvDebugBreak(); skip = true; } \
}
#define BV_ERROR(...) \
{ \
	BvDebugPrint("[BvError]: "); \
	BvDebugPrint(__VA_ARGS__); \
	while (1) { BvDebugBreak(); } \
}
//#else
// Other platforms
//#endif // #if defined(BV_WIN32)

#define BV_VK_CHECK_RESULT(function) \
{ \
	VkResult resultCode = (function); \
	if (resultCode == VkResult::VK_SUCCESS) \
	{ \
		BV_INFO("Vulkan API Call -> %s OK", BV_FUNCTION); \
	} \
	else\
	{ \
		BV_ERROR("Vulkan API Call -> %s Failed - VkResult: %d (%s)", BV_FUNCTION, resultCode, BvVkGetErrorString(resultCode)); \
	} \
}

#else
inline void BvDebugPrint(const char * const format, ...) {}
inline void BvDebugBreak() {}
#define BV_INFO(...) {}
#define BV_WARNING(...) {}
#define BV_ERROR(...) {}
#define BV_VK_CHECK_RESULT(function) (function)
#endif // #if defined(BV_DEBUG)

#pragma warning(disable:4458)
#pragma warning(disable:4221) // No public symbols defined

#define BV_VULKAN_APP_NAME_MAX_LEN 64

// These macros are not the most beautiful thing (they're horrible, actually)
// But they help in avoiding to write repetitive code
#define VK_SAFE_DESTROY_BASE(obj, alloc, type) if (obj) \
{ \
	vkDestroy##type(obj, alloc); \
	obj = VK_NULL_HANDLE; \
}


#define VK_SAFE_DESTROY(device, obj, alloc, type) if (obj) \
{ \
	vkDestroy##type(device, obj, alloc); \
	obj = VK_NULL_HANDLE; \
}


#define VK_SAFE_FREE(device, pool, count, objs, type) if (objs) \
{ \
	vkFree##type##s(device, pool, count, objs); \
	for (size_t i = 0; i < count; ++i) { (objs)[i] = VK_NULL_HANDLE; } \
}


#define VK_SAFE_FREE_MEMORY(device, obj, alloc) if (obj) \
{ \
	vkFreeMemory(device, obj, alloc); \
	obj = VK_NULL_HANDLE; \
}


#define BV_SAFE_DELETE(pObj) if (pObj) \
{ \
	delete pObj; \
	pObj = nullptr; \
}

#define BV_STRINGIFY(value) #value