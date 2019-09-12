#include "VulkanBase.h"
#include "Win32Window.h"
#include "VulkanInstance.h"
#include "VulkanDebugReport.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"


VulkanBase * g_pVulkanBase = nullptr;


VulkanBase::VulkanBase()
{
	g_pVulkanBase = this;
	m_AppName = "Vulkan Demo Base";
}


VulkanBase::~VulkanBase()
{
	g_pVulkanBase = nullptr;
}


void VulkanBase::Run(const VulkanRenderInfo & info)
{
	Create(info);

	m_IsRunning = true;

	MSG msg{};
	while (m_IsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!m_IsPaused)
		{
			Update();
			Render();
		}
	}

	m_pDevice->WaitIdle();

	Destroy();
}


void VulkanBase::Create(const VulkanRenderInfo & info)
{
	SetTextureDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Textures");

	m_pAppRunInfo = new VulkanRenderInfo(info);

	m_Width = info.width;
	m_Height = info.height;

	m_pWindow = new Win32Window();
	if (!m_pWindow->Create(m_AppName.c_str(), DefaultWndProc, m_Width, m_Height, this))
	{
		BV_ERROR("m_pWindow->Create failed");
	}

	m_pInstance = new VulkanInstance();
	m_pInstance->Create(m_AppName.c_str(), info.m_DebugSupport);
	m_pInstance->EnumerateGPUs(m_GPUInfoList);

	m_pDebugReport = new VulkanDebugReport(*m_pInstance);
	m_pDebugReport->Create(this);

	m_pSurface = new VulkanSurface(*m_pInstance);
	m_pSurface->Create(m_pWindow->GetHandle());

	m_pDevice = new VulkanDevice(m_GPUInfoList[0]);
	m_pDevice->Create(m_pSurface, 7U, info.m_DebugSupport);

	m_pGraphicsQueue = m_pDevice->GetGraphicsQueue();
	m_pComputeQueue = m_pDevice->GetComputeQueue();
	m_pTransferQueue = m_pDevice->GetTransferQueue();

	m_pSwapchain = new VulkanSwapchain(*m_pDevice, *m_pSurface);
	m_pSwapchain->Create(m_Width, m_Height, info.m_VSync);

	CreateDefaultCommandPool();
	CreateDefaultCommandBuffers();
	CreateDefaultSemaphores();
	CreateDefaultFences();
}


void VulkanBase::Destroy()
{
	m_Fences.clear();
	BV_SAFE_DELETE(m_pPresentCompleteSemaphore);
	BV_SAFE_DELETE(m_pRenderCompleteSemaphore);
	
	BV_SAFE_DELETE(m_pCommandPool);

	BV_SAFE_DELETE(m_pSwapchain);
	BV_SAFE_DELETE(m_pDevice);
	BV_SAFE_DELETE(m_pSurface);
	BV_SAFE_DELETE(m_pDebugReport);
	BV_SAFE_DELETE(m_pInstance);
	BV_SAFE_DELETE(m_pWindow);
}


void VulkanBase::Resize()
{
	m_pDevice->WaitIdle();

	if (m_pSwapchain)
	{
		m_pSwapchain->Create(m_Width, m_Height, m_pAppRunInfo->m_VSync);
	}
}


void VulkanBase::PrepareFrame()
{
	auto result = m_pSwapchain->Prepare(m_CurrImageIndex, *m_pPresentCompleteSemaphore);

	if (result != VkResult::VK_SUCCESS)
	{
		if (result == VkResult::VK_SUBOPTIMAL_KHR || result == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
		{
			if (!(m_IsMinimized || m_IsResizing))
			{
				Resize();
			}
		}
		else
		{
			BV_VK_CHECK_RESULT(result);
		}
	}
}


void VulkanBase::PresentFrame()
{
	auto result = m_pSwapchain->Present(*m_pRenderCompleteSemaphore);

	if (result != VkResult::VK_SUCCESS)
	{
		if (result == VkResult::VK_SUBOPTIMAL_KHR || result == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
		{
			if (!(m_IsMinimized || m_IsResizing))
			{
				Resize();
			}
		}
		else
		{
			BV_VK_CHECK_RESULT(result);
		}
	}
}


void VulkanBase::CreateDefaultCommandPool()
{
	m_pCommandPool = new VulkanCommandPool(*m_pDevice);
	m_pCommandPool->Create(m_pDevice->GetGraphicsQueue()->GetIndex());
}


void VulkanBase::CreateDefaultCommandBuffers()
{
	m_CommandBuffers.resize(m_pSwapchain->GetImageCount());
	m_pCommandPool->AllocateMultiple(static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
}


void VulkanBase::CreateDefaultSemaphores()
{
	m_pRenderCompleteSemaphore = new VulkanSemaphore(*m_pDevice);
	m_pRenderCompleteSemaphore->Create();

	m_pPresentCompleteSemaphore = new VulkanSemaphore(*m_pDevice);
	m_pPresentCompleteSemaphore->Create();
}


void VulkanBase::CreateDefaultFences()
{
	m_Fences.resize(m_pSwapchain->GetImageCount(), VulkanFence(*m_pDevice));
	for (auto && fence : m_Fences)
	{
		fence.Create(VK_FENCE_CREATE_SIGNALED_BIT);
	}
}


LRESULT WINAPI VulkanBase::DefaultWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			g_pVulkanBase->m_IsPaused = true;
		}
		else
		{
			g_pVulkanBase->m_IsPaused = false;
		}
		return 0;

	case WM_SIZE:
	{
		g_pVulkanBase->m_Width = LOWORD(lParam);
		g_pVulkanBase->m_Height = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			g_pVulkanBase->m_IsMinimized = true;
			g_pVulkanBase->m_IsMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			g_pVulkanBase->Resize();
			g_pVulkanBase->m_IsPaused = false;
			g_pVulkanBase->m_IsMinimized = false;
			g_pVulkanBase->m_IsMaximized = true;
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (g_pVulkanBase->m_IsMinimized)
			{
				g_pVulkanBase->Resize();
				g_pVulkanBase->m_IsPaused = false;
				g_pVulkanBase->m_IsMinimized = false;
			}
			else if (g_pVulkanBase->m_IsMaximized)
			{
				g_pVulkanBase->Resize();
				g_pVulkanBase->m_IsPaused = false;
				g_pVulkanBase->m_IsMaximized = false;
			}
		}
	}
	return 0;

	case WM_ENTERSIZEMOVE:
		g_pVulkanBase->m_IsPaused = true;
		g_pVulkanBase->m_IsResizing = true;
		return 0;

	case WM_EXITSIZEMOVE:
		g_pVulkanBase->Resize();
		g_pVulkanBase->m_IsPaused = false;
		g_pVulkanBase->m_IsResizing = false;
		return 0;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 400;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 400;
		return 0;

	case WM_CLOSE:
		g_pVulkanBase->m_IsRunning = false;
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


void VulkanBase::SetModelDirectory(const std::string & directory)
{
	m_ModelPath = directory;
}


std::string VulkanBase::GetModelFilePath(const std::string & filename)
{
	return m_ModelPath + "\\" + filename;
}


void VulkanBase::SetTextureDirectory(const std::string & directory)
{
	m_TexturePath = directory;
}


std::string VulkanBase::GetTextureFilePath(const std::string & filename)
{
	return m_TexturePath + "\\" + filename;
}


void VulkanBase::SetShaderDirectory(const std::string & directory)
{
	m_ShaderPath = directory;
}


std::string VulkanBase::GetShaderFilePath(const std::string & filename)
{
	return m_ShaderPath + "\\" + filename;
}