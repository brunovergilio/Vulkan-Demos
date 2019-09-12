#include "ClearScreen.h"
#include "ForwardRenderer.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"


ClearScreen::ClearScreen()
{
	m_AppName = "Vulkan Demo - Clear Screen";
}


ClearScreen::~ClearScreen()
{
}


void ClearScreen::Create(const VulkanRenderInfo & info)
{
	VulkanBase::Create(info);

	CreateRenderers();
	RecordCommandBuffers();
}


void ClearScreen::Destroy()
{
	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	VulkanBase::Destroy();
}


void ClearScreen::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void ClearScreen::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void ClearScreen::RecordCommandBuffers()
{
	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.3f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	for (auto i = 0U; i < m_CommandBuffers.size(); ++i)
	{
		m_CommandBuffers[i].BeginCommandBuffer();

		m_CommandBuffers[i].BeginRenderPass(m_pForwardRenderer->GetRenderPass()->Get(), m_pForwardRenderer->GetFrameBuffer()[i], m_Width, m_Height, 2, clearValues);

		m_CommandBuffers[i].SetViewport(static_cast<float>(m_Width), static_cast<float>(m_Height));

		m_CommandBuffers[i].SetScissor(m_Width, m_Height);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void ClearScreen::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore,
		*m_pRenderCompleteSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(ClearScreen)