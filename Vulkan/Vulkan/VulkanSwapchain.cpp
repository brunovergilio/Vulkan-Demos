#include "VulkanSwapchain.h"
#include "VulkanQueue.h"


VulkanSwapchain::VulkanSwapchain(const VulkanDevice & device, const VulkanSurface & surface)
	: m_Device(device)
	, m_Surface(surface)
{
}


VulkanSwapchain::~VulkanSwapchain()
{
	Destroy();
}


VkResult VulkanSwapchain::Create(uint32_t & width, uint32_t & height, bool vSync)
{
	auto & gpuInfo = m_Device.GetGPUInfo();
	auto & physicalDevice = gpuInfo.m_PhysicalDevice;

	m_pPresentationQueue = m_Device.GetPresentationQueue();

	VkResult result;

	// Get list of supported surface formats
	uint32_t formatCount;
	if ((result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr)) != VK_SUCCESS)
	{
		return result;
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	if ((result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data())) != VK_SUCCESS)
	{
	}

	// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
	// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
	if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		m_ColorSpace = surfaceFormats[0].colorSpace;
	}
	else
	{
		// iterate over the list of available surface format and
		// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
		bool found_B8G8R8A8_UNORM = false;
		for (auto & surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormat.format;
				m_ColorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		// in case VK_FORMAT_B8G8R8A8_UNORM is not available
		// select the first available color format
		if (!found_B8G8R8A8_UNORM)
		{
			m_ColorFormat = surfaceFormats[0].format;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
	}

	// Get physical device surface properties and formats
	VkSurfaceCapabilitiesKHR surfCaps;
	if ((result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfCaps)) != VK_SUCCESS)
	{
		return result;
	}

	// Get available present modes
	uint32_t presentModeCount;
	if ((result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr)) != VK_SUCCESS)
	{
		return result;
	}
	assert(presentModeCount > 0);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	if ((result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data())) != VK_SUCCESS)
	{
		return result;
	}

	VkExtent2D swapchainExtent = {};
	// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
	if (surfCaps.currentExtent.width == (uint32_t)-1)
	{
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCaps.currentExtent;
		width = surfCaps.currentExtent.width;
		height = surfCaps.currentExtent.height;
	}

	m_Width = width;
	m_Height = height;

	// Select a present mode for the swapchain

	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank ("v-sync")
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// If v-sync is not requested, try to find a mailbox mode
	// It's the lowest latency non-tearing present mode available
	if (!vSync)
	{
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	// Determine the number of images
	uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
	if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
	{
		desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		// We prefer a non-rotated transform
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfCaps.currentTransform;
	}

	// Find a supported composite alpha format (not all devices support alpha opaque)
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Simply select the first composite alpha format available
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags =
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};

	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	VkSwapchainKHR oldSwapchain = m_Swapchain;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.surface = m_Surface;
	swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
	swapchainCreateInfo.imageFormat = m_ColorFormat;
	swapchainCreateInfo.imageColorSpace = m_ColorSpace;
	swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCreateInfo.imageArrayLayers = 1;

	uint32_t concurrentQueues[2]{};
	{
		uint32_t presentationQueueIndex = m_Device.GetPresentationQueue()->GetIndex();
		uint32_t graphicsQueueIndex = m_Device.GetGraphicsQueue()->GetIndex();

		if (graphicsQueueIndex == presentationQueueIndex)
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		else
		{
			concurrentQueues[0] = graphicsQueueIndex;
			concurrentQueues[1] = presentationQueueIndex;

			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = concurrentQueues;
		}
	}
	swapchainCreateInfo.presentMode = swapchainPresentMode;
	swapchainCreateInfo.oldSwapchain = oldSwapchain;
	// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.compositeAlpha = compositeAlpha;

	// Enable transfer source on swap chain images if supported
	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	// Enable transfer destination on swap chain images if supported
	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	if ((result = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &m_Swapchain)) != VK_SUCCESS)
	{
		return result;
	}

	// If an existing swap chain is re-created, destroy the old swap chain
	// This also cleans up all the presentable images
	if (oldSwapchain != VK_NULL_HANDLE)
	{
		for (size_t i = 0; i < m_SwapchainViews.size(); i++)
		{
			vkDestroyImageView(m_Device, m_SwapchainViews[i], nullptr);
		}
		m_SwapchainViews.clear();
		m_SwapchainImages.clear();
		vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
	}
	uint32_t imageCount = 0;
	if ((result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr)) != VK_SUCCESS)
	{
		return result;
	}

	// Get the swap chain images
	m_SwapchainImages.resize(imageCount);
	m_SwapchainViews.resize(imageCount);
	if ((result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_SwapchainImages.data())) != VK_SUCCESS)
	{
		return result;
	}

	// Get the swap chain buffers containing the image and imageview
	for (uint32_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = nullptr;
		colorAttachmentView.format = m_ColorFormat;
		colorAttachmentView.components =
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.flags = 0;

		colorAttachmentView.image = m_SwapchainImages[i];

		if ((result = vkCreateImageView(m_Device, &colorAttachmentView, nullptr, &m_SwapchainViews[i])) != VK_SUCCESS)
		{
			return result;
		}
	}

	return VkResult::VK_SUCCESS;
}


void VulkanSwapchain::Destroy()
{
	for (size_t i = 0; i < m_SwapchainViews.size(); i++)
	{
		VK_SAFE_DESTROY(m_Device, m_SwapchainViews[i], nullptr, ImageView);
	}

	VK_SAFE_DESTROY(m_Device, m_Swapchain, nullptr, SwapchainKHR);
}


VkResult VulkanSwapchain::Prepare(uint32_t & currImageIndex, const VkSemaphore semaphore, const VkFence fence)
{
	auto result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, semaphore, fence, &m_CurrImageIndex);
	currImageIndex = m_CurrImageIndex;

	return result;
}


VkResult VulkanSwapchain::Present(const VkSemaphore waitSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &m_CurrImageIndex;
	// Check if a wait semaphore has been specified to wait for before presenting the image
	if (waitSemaphore)
	{
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
	}
	
	return m_pPresentationQueue->Present(presentInfo);
}