#pragma once


#include "VulkanDevice.h"


struct FramebufferCreateInfo : VkFramebufferCreateInfo
{
	FramebufferCreateInfo(const VkRenderPass renderPass, const uint32_t attachmentCount, const VkImageView * const pAttachments,
		const uint32_t width, const uint32_t height, const uint32_t layers = 1)
	{
		this->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		this->pNext = nullptr;
		this->flags = 0;
		this->renderPass = renderPass;
		this->attachmentCount = attachmentCount;
		this->pAttachments = pAttachments;
		this->width = width;
		this->height = height;
		this->layers = layers;
	}

	FramebufferCreateInfo() = default;
};


class VulkanFramebuffer
{
public:
	explicit VulkanFramebuffer(const VulkanDevice & device);
	~VulkanFramebuffer();

	VkResult Create(const VkFramebufferCreateInfo & createInfo);
	void Destroy();

	inline const VkFramebuffer Get() const { return m_FrameBuffer; }
	inline operator VkFramebuffer() const { return m_FrameBuffer; }

private:
	const VulkanDevice & m_Device;
	VkFramebuffer m_FrameBuffer = VK_NULL_HANDLE;
};