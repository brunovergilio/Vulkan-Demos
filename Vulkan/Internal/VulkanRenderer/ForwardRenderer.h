#pragma once


#include "VulkanSwapchain.h"


class VulkanFramebuffer;
class VulkanRenderPass;
class VulkanTexture;


class ForwardRenderer
{
public:
	ForwardRenderer();
	~ForwardRenderer();

	void Create();
	void Destroy();
	void Resize();

	inline const VulkanRenderPass * const GetRenderPass() const { return m_pRenderPass; }
	inline const VulkanFramebuffer * const GetFrameBuffer() const { return m_FrameBuffers.data(); }

private:
	void CreateDepthStencil(const uint32_t width, const uint32_t height);
	void CreateRenderPass();
	void CreateFrameBuffers(const uint32_t width, const uint32_t height);

public:
	VulkanTexture * m_pDepthStencilTexture = nullptr;
	VulkanRenderPass * m_pRenderPass = nullptr;
	std::vector<VulkanFramebuffer> m_FrameBuffers;
	VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;
};