#pragma once


#include "VulkanDevice.h"


struct ImageViewCreateInfo : VkImageViewCreateInfo
{
	ImageViewCreateInfo(const VkImage image, const VkImageViewType viewType, const VkFormat format,
		const VkImageAspectFlags aspectMask, const uint32_t levelCount = 1, const uint32_t layerCount = 1,
		const VkComponentMapping & components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })
		: VkImageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, image, viewType, format, components,
			{ aspectMask, 0, levelCount, 0, layerCount } }
	{
	}
};


class VulkanImageView
{
public:
	VulkanImageView() = default;
	explicit VulkanImageView(const VulkanDevice & device);
	~VulkanImageView();

	VkResult Create(const VkImageViewCreateInfo & imageViewCreateInfo);
	void Destroy();

	inline const VkImageView Get() const { return m_View; }
	inline operator VkImageView() const { return m_View; }

private:
	const VulkanDevice & m_Device;
	VkImageView m_View = VK_NULL_HANDLE;
};