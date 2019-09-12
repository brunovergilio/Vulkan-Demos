#include "VulkanImage.h"


VulkanImage::VulkanImage(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanImage::~VulkanImage()
{
	Destroy();
}


VkResult VulkanImage::Create(const VkImageCreateInfo & imageCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
{
	VkResult result;
	if ((result = vkCreateImage(m_Device, &imageCreateInfo, nullptr, &m_Image)) != VK_SUCCESS)
	{
		return result;
	}

	VkMemoryRequirements reqs{};
	vkGetImageMemoryRequirements(m_Device, m_Image, &reqs);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = reqs.size;
	allocateInfo.memoryTypeIndex = m_Device.GetMemoryTypeIndex(reqs.memoryTypeBits,
		(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) ? memoryPropertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
		: memoryPropertyFlags);

	if ((result = vkAllocateMemory(m_Device, &allocateInfo, nullptr, &m_Memory)) != VK_SUCCESS)
	{
		VK_SAFE_DESTROY(m_Device, m_Image, nullptr, Image);

		return result;
	}

	if ((result = vkBindImageMemory(m_Device, m_Image, m_Memory, 0)) != VK_SUCCESS)
	{
		VK_SAFE_FREE_MEMORY(m_Device, m_Memory, nullptr);
		VK_SAFE_DESTROY(m_Device, m_Image, nullptr, Image);

		return result;
	}

	return VK_SUCCESS;
}


void VulkanImage::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Image, nullptr, Image);
	VK_SAFE_FREE_MEMORY(m_Device, m_Memory, nullptr);
}