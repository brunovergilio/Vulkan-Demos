#pragma once


#include "VulkanDevice.h"
#include "VulkanSurface.h"


class VulkanQueue;

class VulkanSwapchain
{
public:
	explicit VulkanSwapchain(const VulkanDevice & device, const VulkanSurface & surface);
	~VulkanSwapchain();

	VkResult Create(uint32_t & width, uint32_t & height, bool vSync = false);
	void Destroy();

	VkResult Prepare(uint32_t & currImageIndex, const VkSemaphore semaphore, const VkFence fence = VK_NULL_HANDLE);
	VkResult Present(const VkSemaphore waitSemaphore);

	inline const uint32_t GetImageCount() const { return (uint32_t)m_SwapchainViews.size(); }
	inline const VkImageView * GetViews() const { return m_SwapchainViews.data(); }
	inline const VkImage * GetImages() const { return m_SwapchainImages.data(); }
	inline const VkFormat GetColorFormat() const { return m_ColorFormat; }
	inline const uint32_t GetCurrImage() const { return m_CurrImageIndex; }
	inline const void GetDimensions(uint32_t & width, uint32_t & height) const { width = m_Width; height = m_Height; }

	inline const VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
	inline operator VkSwapchainKHR() const { return m_Swapchain; }

private:
	const VulkanDevice & m_Device;
	const VulkanSurface & m_Surface;

	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainViews;

	VulkanQueue * m_pPresentationQueue = nullptr;

	VkColorSpaceKHR m_ColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
	VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
	uint32_t m_CurrImageIndex = 0;
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
};