#pragma once


#include "VulkanDevice.h"


struct AttachmentReference : VkAttachmentReference
{
	AttachmentReference(const uint32_t attachment, const VkImageLayout layout)
		: VkAttachmentReference{ attachment, layout }
	{
	}
};


struct RenderPassCreateInfo : VkRenderPassCreateInfo
{
	std::vector<VkAttachmentDescription> m_Attachments;
	std::vector<VkSubpassDescription> m_Subpasses;
	std::vector<VkSubpassDependency> m_Dependencies;


	RenderPassCreateInfo()
		: VkRenderPassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr, 0, nullptr}
	{
	}

	void AddAttachment(const VkFormat format, const VkImageLayout finalLayout,
		const VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, const VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		const VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, const VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, const VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		const VkAttachmentDescriptionFlags flags = 0)
	{
		m_Attachments.push_back({ flags, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout });

		attachmentCount = static_cast<uint32_t>(m_Attachments.size());
		pAttachments = m_Attachments.data();
	}
	
	void ClearAttachments()
	{
		m_Attachments.clear();

		attachmentCount = 0;
		pAttachments = nullptr;
	}

	void AddSubpass(const uint32_t colorAttachmentCount, const VkAttachmentReference * const pColorAttachments, const VkAttachmentReference * const pDepthStencilAttachment,
		const uint32_t inputAttachmentCount = 0, const VkAttachmentReference * const pInputAttachments = nullptr, const VkAttachmentReference * const pResolveAttachments = nullptr,
		const uint32_t preserveAttachmentCount = 0, const uint32_t * const pPreserveAttachments = nullptr)
	{
		m_Subpasses.push_back({ 0, VK_PIPELINE_BIND_POINT_GRAPHICS, inputAttachmentCount, pInputAttachments, colorAttachmentCount,
			pColorAttachments, pResolveAttachments, pDepthStencilAttachment, preserveAttachmentCount, pPreserveAttachments });

		subpassCount = static_cast<uint32_t>(m_Subpasses.size());
		pSubpasses = m_Subpasses.data();
	}

	void ClearSubpasses()
	{
		m_Subpasses.clear();

		subpassCount = 0;
		pSubpasses = nullptr;
	}

	void AddDependency(const uint32_t srcSubpass, const uint32_t dstSubpass,
		const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask,
		const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
		const VkDependencyFlags dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT)
	{
		m_Dependencies.push_back({ srcSubpass, dstSubpass, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags });

		dependencyCount = static_cast<uint32_t>(m_Dependencies.size());
		pDependencies = m_Dependencies.data();
	}

	void ClearDependencies()
	{
		m_Dependencies.clear();

		dependencyCount = 0;
		pDependencies = nullptr;
	}
};


class VulkanRenderPass
{
public:
	explicit VulkanRenderPass(const VulkanDevice & device);
	~VulkanRenderPass();

	VkResult Create(const VkRenderPassCreateInfo & renderPassCreateInfo);
	void Destroy();

	inline const VkRenderPass Get() const { return m_RenderPass; }
	inline operator VkRenderPass() const { return m_RenderPass; }

private:
	const VulkanDevice & m_Device;
	VkRenderPass m_RenderPass = VK_NULL_HANDLE;
};