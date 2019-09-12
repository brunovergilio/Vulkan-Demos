#pragma once


#include "VulkanUtils.h"


struct ImageBarrier : VkImageMemoryBarrier
{
	ImageBarrier(const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
		const VkImageLayout oldLayout, const VkImageLayout newLayout,
		const VkImage image, const VkImageSubresourceRange & subresourceRange,
		const uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		const uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED)
		: VkImageMemoryBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, srcAccessMask, dstAccessMask, oldLayout, newLayout,
			srcQueueFamilyIndex, dstQueueFamilyIndex, image, subresourceRange }
	{}
};