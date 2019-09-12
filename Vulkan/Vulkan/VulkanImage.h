#pragma once


#include "VulkanDevice.h"


struct ImageCreateInfo : VkImageCreateInfo
{
	ImageCreateInfo(const VkImageType imageType, const VkFormat format, const VkImageUsageFlags usage, const VkExtent3D & extent,
		const uint32_t mipLevels = 1, const uint32_t arrayLayers = 1, const VkImageCreateFlags flags = 0)
		: VkImageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, flags, imageType, format, extent, mipLevels, arrayLayers,
			VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_UNDEFINED }
	{
	}
};


class VulkanImage
{
public:
	VulkanImage() = default;
	explicit VulkanImage(const VulkanDevice & device);
	~VulkanImage();

	VkResult Create(const VkImageCreateInfo & imageCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	void Destroy();

	inline const VkImage Get() const { return m_Image; }
	inline operator VkImage() const { return m_Image; }

private:
	const VulkanDevice & m_Device;
	VkImage m_Image = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};