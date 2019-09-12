#include "ForwardRenderer.h"
#include "VulkanBase.h"
#include "VulkanTexture.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanSwapchain.h"


ForwardRenderer::ForwardRenderer()
{
}


ForwardRenderer::~ForwardRenderer()
{
}


void ForwardRenderer::Create()
{
	auto pDevice = g_pVulkanBase->GetDevice();
	auto pSwapchain = g_pVulkanBase->GetSwapchain();

	m_pDepthStencilTexture = new VulkanTexture();
	m_pRenderPass = new VulkanRenderPass(*pDevice);

	uint32_t width, height;
	pSwapchain->GetDimensions(width, height);

	CreateDepthStencil(width, height);
	CreateRenderPass();
	CreateFrameBuffers(width, height);
}


void ForwardRenderer::Destroy()
{
	for (auto && fb : m_FrameBuffers)
	{
		fb.Destroy();
	}

	m_pRenderPass->Destroy();
	m_pDepthStencilTexture->Destroy();

	m_FrameBuffers.clear();
	BV_SAFE_DELETE(m_pDepthStencilTexture);
	BV_SAFE_DELETE(m_pRenderPass);
}


void ForwardRenderer::Resize()
{
	for (auto && frameBuffer : m_FrameBuffers)
	{
		frameBuffer.Destroy();
	}

	m_pDepthStencilTexture->Destroy();

	uint32_t width, height;
	g_pVulkanBase->GetSwapchain()->GetDimensions(width, height);

	CreateDepthStencil(width, height);
	CreateFrameBuffers(width, height);
}


void ForwardRenderer::CreateDepthStencil(const uint32_t width, const uint32_t height)
{
	m_DepthFormat = g_pVulkanBase->GetDevice()->GetBestDepthFormat();
	BV_ASSERT(m_DepthFormat != VK_FORMAT_UNDEFINED);

	TextureCreateInfo textureCreateInfo(width, height, m_DepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	m_pDepthStencilTexture->Create(textureCreateInfo);
}


void ForwardRenderer::CreateRenderPass()
{
	const VulkanSwapchain * const pSwapchain = g_pVulkanBase->GetSwapchain();
	BV_ASSERT(pSwapchain != nullptr);

	AttachmentReference colorRef(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	AttachmentReference depthRef(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.AddAttachment(pSwapchain->GetColorFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	renderPassCreateInfo.AddAttachment(m_DepthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassCreateInfo.m_Attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

	renderPassCreateInfo.AddSubpass(1, &colorRef, &depthRef);
	renderPassCreateInfo.AddDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	renderPassCreateInfo.AddDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);

	m_pRenderPass->Create(renderPassCreateInfo);
}


void ForwardRenderer::CreateFrameBuffers(const uint32_t width, const uint32_t height)
{
	VulkanSwapchain * pSwapchain = g_pVulkanBase->GetSwapchain();

	m_FrameBuffers.resize(pSwapchain->GetImageCount(), VulkanFramebuffer(*g_pVulkanBase->GetDevice()));
	for (auto i = 0U; i < m_FrameBuffers.size(); ++i)
	{
		VkImageView views[] = { pSwapchain->GetViews()[i], *m_pDepthStencilTexture->GetView() };
		FramebufferCreateInfo frameBufferCreateInfo(m_pRenderPass->Get(), 2, views, width, height);
		m_FrameBuffers[i].Create(frameBufferCreateInfo);
	}
}