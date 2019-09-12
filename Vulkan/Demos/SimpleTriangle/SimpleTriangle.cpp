#include "SimpleTriangle.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanShader.h"
#include "VulkanPipelineCache.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "ForwardRenderer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanSemaphore.h"


SimpleTriangle::SimpleTriangle()
{
	m_AppName = "Vulkan Demo - Simple Triangle";
}


SimpleTriangle::~SimpleTriangle()
{
}


void SimpleTriangle::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\SimpleTriangle");

	VulkanBase::Create(info);

	CreateRenderers();
	CreatePipelineLayouts();
	CreatePipelineCache();
	CreateGraphicsPipelines();
	RecordCommandBuffers();
}


void SimpleTriangle::Destroy()
{
	m_pPipeline->Destroy(); BV_SAFE_DELETE(m_pPipeline);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayout->Destroy(); BV_SAFE_DELETE(m_pPipelineLayout);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	VulkanBase::Destroy();
}


void SimpleTriangle::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void SimpleTriangle::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void SimpleTriangle::CreatePipelineLayouts()
{
	PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	m_pPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
	m_pPipelineLayout->Create(pipelineLayoutCreateInfo);
}


void SimpleTriangle::CreatePipelineCache()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void SimpleTriangle::CreateGraphicsPipelines()
{
	auto vsShader = GetShaderFilePath("vs.spv");
	auto fsShader = GetShaderFilePath("fs.spv");

	VulkanShader vs(*m_pDevice), fs(*m_pDevice);
	vs.Create(vsShader.c_str());
	fs.Create(fsShader.c_str());

	GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	// No vertex bindings
	// No vertex attributes
	// No tessellation
	// Default rasterizer already set in constructor
	// Default depth / stencil already set in constructor
	// Default Multisampling already set in constructor
	graphicsPipelineCreateInfo.AddBlendAttachment();
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs.Get());
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs.Get());
	graphicsPipelineCreateInfo.SetRenderInfo(m_pPipelineLayout->Get(), m_pForwardRenderer->GetRenderPass()->Get(), 0);

	m_pPipeline = new VulkanPipeline(*m_pDevice);
	m_pPipeline->Create(graphicsPipelineCreateInfo, m_pPipelineCache->Get());

	vs.Destroy();
	fs.Destroy();
}


void SimpleTriangle::RecordCommandBuffers()
{
	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.3f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	for (auto i = 0U; i < m_CommandBuffers.size(); ++i)
	{
		m_CommandBuffers[i].BeginCommandBuffer();

		m_CommandBuffers[i].BeginRenderPass(*m_pForwardRenderer->GetRenderPass(), m_pForwardRenderer->GetFrameBuffer()[i], m_Width, m_Height, 2, clearValues);

		m_CommandBuffers[i].SetViewport(static_cast<float>(m_Width), static_cast<float>(m_Height));

		m_CommandBuffers[i].SetScissor(m_Width, m_Height);

		m_CommandBuffers[i].BindPipeline(m_pPipeline->Get());

		m_CommandBuffers[i].Draw(3);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void SimpleTriangle::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(SimpleTriangle)