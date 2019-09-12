#include "Win32Window.h"



Win32Window::Win32Window()
{
}


Win32Window::~Win32Window()
{
	Destroy();
}


bool Win32Window::Create(const char * const pName, WNDPROC pWndProc, const uint32_t width, const uint32_t height, void * const pData)
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	// Register the window class.
	WNDCLASSEX wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = pWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = pName;
	wndClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		return false;
	}

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = width;
	windowRect.bottom = height;

	DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

	m_hWnd = CreateWindow(pName, pName, style, 0, 0,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, pData);

	if (!m_hWnd)
	{
		UnregisterClass(pName, hInstance);
		return false;
	}
	else
	{
		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
		SetWindowText(m_hWnd, pName);
	}

	return true;
}


void Win32Window::Destroy()
{
	if (m_hWnd)
	{
		char appName[BV_VULKAN_APP_NAME_MAX_LEN];
		GetClassName(m_hWnd, appName, BV_VULKAN_APP_NAME_MAX_LEN);

		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
		UnregisterClass(appName, GetModuleHandle(nullptr));
	}
}