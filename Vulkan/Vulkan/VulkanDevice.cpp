#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanSurface.h"


VulkanDevice::VulkanDevice(const VulkanGPUInfo & gpuInfo)
	: m_GPUInfo(gpuInfo)
{
}


VulkanDevice::~VulkanDevice()
{
	Destroy();
}


VkResult VulkanDevice::Create(VulkanSurface * pSurface, const VkFlags queues, const bool debugSupport)
{
	VkResult result;

	uint32_t graphicsQueueIndex = queues & VK_QUEUE_GRAPHICS_BIT ? GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT) : UINT32_MAX;
	uint32_t computeQueueIndex = queues & VK_QUEUE_COMPUTE_BIT ? GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT) : UINT32_MAX;
	uint32_t transferQueueIndex = queues & VK_QUEUE_TRANSFER_BIT ? GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT) : UINT32_MAX;
	uint32_t presentationQueueIndex = UINT32_MAX;

	if (pSurface)
	{
		VkBool32 supported = false;
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_GPUInfo.m_QueueFamilyProperties.size()); i++)
		{
			if ((result = vkGetPhysicalDeviceSurfaceSupportKHR(m_GPUInfo.m_PhysicalDevice, i, *pSurface, &supported)) != VK_SUCCESS)
			{
				return result;
			}

			if (supported)
			{
				presentationQueueIndex = i;

				// If the presentation queue is the same as the graphics, then we stop looking for other options
				if (presentationQueueIndex == graphicsQueueIndex)
				{
					break;
				}
			}
		}

		// If we didn't find anything, then return an error
		if (presentationQueueIndex == UINT32_MAX)
		{
			return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	// ===========================================================
	// Prepare Device Queues and create the logical device
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	const float queuePriority = 0.0f;
	VkDeviceQueueCreateInfo queueCreateInfo{};

	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
	queueInfos.push_back(queueCreateInfo);

	if (computeQueueIndex != graphicsQueueIndex)
	{
		queueCreateInfo.queueFamilyIndex = computeQueueIndex;
		queueInfos.push_back(queueCreateInfo);
	}

	if (transferQueueIndex != graphicsQueueIndex
		&& transferQueueIndex != computeQueueIndex)
	{
		queueCreateInfo.queueFamilyIndex = transferQueueIndex;
		queueInfos.push_back(queueCreateInfo);
	}

	if (presentationQueueIndex != graphicsQueueIndex
		&& presentationQueueIndex != computeQueueIndex
		&& presentationQueueIndex != transferQueueIndex)
	{
		queueCreateInfo.queueFamilyIndex = presentationQueueIndex;
		queueInfos.push_back(queueCreateInfo);
	}

	std::vector<const char *> enabledExtensions;
	if (pSurface)
	{
		enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	if (debugSupport)
	{
		if (std::find(m_GPUInfo.m_SupportedExtensions.cbegin(), m_GPUInfo.m_SupportedExtensions.cend(), VK_EXT_DEBUG_MARKER_EXTENSION_NAME)
			!= m_GPUInfo.m_SupportedExtensions.cend())
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}
	}

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
	deviceCreateInfo.pEnabledFeatures = &m_GPUInfo.m_DeviceFeatures;

	if ((result = vkCreateDevice(m_GPUInfo.m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device)) != VK_SUCCESS)
	{
		return result;
	}

	m_pGraphicsQueue = graphicsQueueIndex != UINT32_MAX ? new VulkanQueue(*this, graphicsQueueIndex) : nullptr;
	m_pComputeQueue = computeQueueIndex != UINT32_MAX ? new VulkanQueue(*this, computeQueueIndex) : nullptr;
	m_pTransferQueue = transferQueueIndex != UINT32_MAX ? new VulkanQueue(*this, transferQueueIndex) : nullptr;
	m_pPresentationQueue = presentationQueueIndex != UINT32_MAX ? new VulkanQueue(*this, presentationQueueIndex) : nullptr;

	return VK_SUCCESS;
}


void VulkanDevice::Destroy()
{
	BV_SAFE_DELETE(m_pGraphicsQueue);
	BV_SAFE_DELETE(m_pComputeQueue);
	BV_SAFE_DELETE(m_pTransferQueue);
	BV_SAFE_DELETE(m_pPresentationQueue);

	VK_SAFE_DESTROY_BASE(m_Device, nullptr, Device);
}


VkResult VulkanDevice::WaitIdle()
{
	return vkDeviceWaitIdle(m_Device);
}


const uint32_t VulkanDevice::GetQueueFamilyIndex(const VkQueueFlags queueFlags) const
{
	auto & queueFamilyProperties = m_GPUInfo.m_QueueFamilyProperties;

	uint32_t index = UINT32_MAX;

	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & queueFlags)
				&& ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				index = i;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	// Try to find a queue family index that supports transfer but not graphics and compute
	else if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & queueFlags)
				&& ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
				&& ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				index = i;
				break;
			}
		}
	}


	if (UINT32_MAX == index)
	{
		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if (queueFamilyProperties[i].queueFlags & queueFlags)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}


const uint32_t VulkanDevice::GetMemoryTypeIndex(const uint32_t memoryTypeBits, const VkMemoryPropertyFlags properties) const
{
	uint32_t index = UINT32_MAX;

	uint32_t typeBits = memoryTypeBits;

	auto & memoryProperties = m_GPUInfo.m_DeviceMemoryProperties;
	// Iterate over all memory types available for the device used in this example
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				index = i;
				break;
			}
		}
		typeBits >>= 1;
	}

	return index;
}


const VkFormat VulkanDevice::GetBestDepthFormat(const VkFormat format) const
{
	VkFormat chosenFormat = VK_FORMAT_UNDEFINED;

	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	VkFormat depthFormats[] =
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	auto & physicalDevice = m_GPUInfo.m_PhysicalDevice;
	VkFormatProperties formatProps;
	for (auto & depthFormat : depthFormats)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormat, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			chosenFormat = depthFormat;
			if (format == VK_FORMAT_UNDEFINED || format == chosenFormat)
			{
				break;
			}
		}
	}

	return chosenFormat;
}
