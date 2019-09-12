#pragma once


#include "VulkanInstance.h"


class VulkanSurface
{
public:
	VulkanSurface(const VulkanInstance & instance);
	~VulkanSurface();

	VkResult Create(const HWND hWnd);
	void Destroy();

	inline const VkSurfaceKHR Get() const { return m_Surface; }
	inline operator VkSurfaceKHR() const { return m_Surface; }

private:
	const VulkanInstance & m_Instance;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
};