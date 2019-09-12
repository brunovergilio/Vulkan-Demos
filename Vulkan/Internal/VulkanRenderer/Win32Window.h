#pragma once


#include "VulkanUtils.h"


class Win32Window
{
public:
	Win32Window();
	~Win32Window();

	bool Create(const char * const pName, WNDPROC pWndProc, const uint32_t width, const uint32_t height, void * const pData);
	void Destroy();

	inline const HWND GetHandle() const { return m_hWnd; }

private:
	HWND m_hWnd = nullptr;
};