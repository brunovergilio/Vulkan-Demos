#include "VulkanSurface.h"


VulkanSurface::VulkanSurface(const VulkanInstance & instance)
	: m_Instance(instance)
{
}


VulkanSurface::~VulkanSurface()
{
	Destroy();
}


VkResult VulkanSurface::Create(const HWND hWnd)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	surfaceCreateInfo.hwnd = hWnd;

	return vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface);
}


void VulkanSurface::Destroy()
{
	VK_SAFE_DESTROY(m_Instance, m_Surface, nullptr, SurfaceKHR);
}