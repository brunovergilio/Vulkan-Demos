#pragma once


#include "VulkanUtils.h"
#include "VulkanDebugReport.h"
#include "VulkanFence.h"


class VulkanDebugReport;
class VulkanDevice;
class VulkanSwapchain;
class VulkanSurface;
class Win32Window;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanSemaphore;

#define BV_VULKAN_APP_NAME_MAX_LEN 64

#define CREATE_WIN_MAIN(obj) \
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) \
{ \
	obj demo; \
	demo.Run(); \
\
	return 0; \
}


// To do: Implement this struct and add more features
struct VulkanRenderInfo
{
	uint32_t m_PhysicalDeviceIndex = 0;
	uint32_t width = 800;
	uint32_t height = 600;
	bool m_DebugSupport = true;
	bool m_VSync = true;
};


class VulkanBase
{
public:
	VulkanBase();
	~VulkanBase();

	void Run(const VulkanRenderInfo & info = VulkanRenderInfo());

	inline VulkanDevice * const GetDevice() const { return m_pDevice; }
	inline VulkanSwapchain * const GetSwapchain() const { return m_pSwapchain; }
	inline VulkanCommandPool * const GetDefaultCommandPool() const { return m_pCommandPool; }

	void SetModelDirectory(const std::string & directory);
	std::string GetModelFilePath(const std::string & filename);
	void SetTextureDirectory(const std::string & directory);
	std::string GetTextureFilePath(const std::string & filename);
	void SetShaderDirectory(const std::string & directory);
	std::string GetShaderFilePath(const std::string & filename);

protected:
	virtual void Create(const VulkanRenderInfo & info);
	virtual void Destroy();

	virtual void Resize();

	void PrepareFrame();
	void PresentFrame();

	void CreateDefaultCommandPool();
	void CreateDefaultCommandBuffers();
	void CreateDefaultSemaphores();
	void CreateDefaultFences();

	// Pure Virtual Methods
	virtual void Update() = 0;
	virtual void Render() = 0;

private:
	// Windows callback routine
	static LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	// App Name
	std::string m_AppName;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	VulkanRenderInfo * m_pAppRunInfo = nullptr;

	// Window Handle
	Win32Window * m_pWindow = nullptr;

	// Vulkan API
	VulkanInstance * m_pInstance = nullptr;
	VulkanDebugReport * m_pDebugReport = nullptr;
	VulkanSurface * m_pSurface = nullptr;
	VulkanDevice * m_pDevice = nullptr;
	VulkanSwapchain * m_pSwapchain = nullptr;
	VulkanGPUInfoList m_GPUInfoList;

	VulkanQueue * m_pGraphicsQueue = nullptr;
	VulkanQueue * m_pComputeQueue = nullptr;
	VulkanQueue * m_pTransferQueue = nullptr;

	// Default Objects
	VulkanCommandPool * m_pCommandPool = nullptr;
	std::vector<VulkanCommandBuffer> m_CommandBuffers;

	VulkanSemaphore * m_pRenderCompleteSemaphore;
	VulkanSemaphore * m_pPresentCompleteSemaphore;
	std::vector<VulkanFence> m_Fences;

	uint32_t m_CurrImageIndex = 0;

	std::string m_ModelPath;
	std::string m_TexturePath;
	std::string m_ShaderPath;

	bool m_IsRunning = false;
	bool m_IsPaused = false;
	bool m_IsResizing = false;
	bool m_IsMinimized = false;
	bool m_IsMaximized = false;
};


extern VulkanBase * g_pVulkanBase;