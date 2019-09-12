#include "TriangleAndBuffers.h"
#include "ForwardRenderer.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanPipelineCache.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanShader.h"
#include "VertexLayout.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "Primitives.h"


TriangleAndBuffers::TriangleAndBuffers()
{
	m_AppName = "Vulkan Demo - Triangles and Buffers";
}


TriangleAndBuffers::~TriangleAndBuffers()
{
}


void TriangleAndBuffers::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\TriangleAndBuffers");

	VulkanBase::Create(info);

	CreateRenderers();
	CreateBuffers();
	CreateUniformBuffers();
	CreateDescriptorPools();
	CreateDescriptorSetLayouts();
	CreateDescriptorSets();
	CreatePipelineLayouts();
	CreatePipelineCaches();
	CreateGraphicsPipelines();
	RecordCommandBuffers();
}


void TriangleAndBuffers::Destroy()
{
	m_pPipeline->Destroy(); BV_SAFE_DELETE(m_pPipeline);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayout->Destroy(); BV_SAFE_DELETE(m_pPipelineLayout);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	BV_SAFE_DELETE(m_pDescriptorSet);
	m_pDescriptorSetLayout->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayout);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	m_pUB->Destroy(); BV_SAFE_DELETE(m_pUB);
	m_pVB->Destroy(); BV_SAFE_DELETE(m_pVB);
	m_pIB->Destroy(); BV_SAFE_DELETE(m_pIB);

	VulkanBase::Destroy();
}


void TriangleAndBuffers::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void TriangleAndBuffers::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void TriangleAndBuffers::CreateBuffers()
{
	BvVertexList vertices;
	BvIndexList<uint32_t> indices;

	BvVertexLayout layouts[] =
	{
		BV_VERTEX_POSITION,
		BV_VERTEX_COLOR
	};

	CreateBox(BV_VERTEX_POSITION | BV_VERTEX_COLOR, vertices, indices);
	m_NumIndices = indices.size();

	VulkanBuffer tmpVB(*m_pDevice);
	VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
	tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vbSize);
	tmpVB.Map();
	tmpVB.Update(vertices.data(), vbSize);
	tmpVB.Unmap();

	m_pVB = new VulkanBuffer(*m_pDevice);
	m_pVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


	VulkanBuffer tmpIB(*m_pDevice);
	VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
	tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		ibSize);
	tmpIB.Map();
	tmpIB.Update(indices.data(), ibSize);
	tmpIB.Unmap();

	m_pIB = new VulkanBuffer(*m_pDevice);
	m_pIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

	VulkanFence fence(*m_pDevice);
	fence.Create();

	VulkanCommandBuffer cb;
	m_pCommandPool->Allocate(&cb);

	cb.BeginCommandBuffer();
	cb.CopyBuffer(tmpVB, *m_pVB, vbSize);
	cb.CopyBuffer(tmpIB, *m_pIB, ibSize);
	cb.EndCommandBuffer();

	m_pGraphicsQueue->Submit(cb, VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence);

	fence.Wait();
	
	tmpVB.Destroy();
	tmpIB.Destroy();
	fence.Destroy();

	m_pCommandPool->Free(&cb);
}


void TriangleAndBuffers::CreateUniformBuffers()
{
	m_pUB = new VulkanBuffer(*m_pDevice);
	m_pUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float44));
	m_pUB->Map();
}


void TriangleAndBuffers::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void TriangleAndBuffers::CreateDescriptorSetLayouts()
{
	DescriptorSetLayoutCreateInfo setLayoutInfo;
	setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_pDescriptorSetLayout = new VulkanDescriptorSetLayout(*m_pDevice);
	m_pDescriptorSetLayout->Create(setLayoutInfo);
}


void TriangleAndBuffers::CreateDescriptorSets()
{
	m_pDescriptorSet = new VulkanDescriptorSet(*m_pDevice);
	m_pDescriptorPool->Allocate(m_pDescriptorSet, m_pDescriptorSetLayout);

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = *m_pUB;
	bufferInfo.range = sizeof(Float44);

	WriteDescriptorSet writeSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
	m_pDescriptorSet->Update(1, &writeSet);
}


void TriangleAndBuffers::CreatePipelineLayouts()
{
	PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayout);
	m_pPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
	m_pPipelineLayout->Create(pipelineLayoutCreateInfo);
}


void TriangleAndBuffers::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void TriangleAndBuffers::CreateGraphicsPipelines()
{
	auto vsShader = GetShaderFilePath("vs.spv");
	auto fsShader = GetShaderFilePath("fs.spv");

	VulkanShader vs(*m_pDevice), fs(*m_pDevice);
	vs.Create(vsShader.c_str());
	fs.Create(fsShader.c_str());

	GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	graphicsPipelineCreateInfo.AddVertexBinding(0, g_BvVertexSize * 7, VK_VERTEX_INPUT_RATE_VERTEX);
	graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	graphicsPipelineCreateInfo.AddVertexAttribute(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, g_BvVertexSize * 3);
	// No tessellation
	// Default rasterizer already set in constructor
	// Default depth / stencil already set in constructor
	// Default Multisampling already set in constructor
	graphicsPipelineCreateInfo.AddBlendAttachment();
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
	graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayout, *m_pForwardRenderer->GetRenderPass(), 0);

	m_pPipeline = new VulkanPipeline(*m_pDevice);
	m_pPipeline->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

	vs.Destroy();
	fs.Destroy();
}


void TriangleAndBuffers::RecordCommandBuffers()
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

		m_CommandBuffers[i].BindPipeline(*m_pPipeline);

		VkDescriptorSet sets[] = { *m_pDescriptorSet };
		m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayout, 0, 1, sets);

		VkDeviceSize offsets[] = { 0 };
		VkBuffer PosColorVertexBuffers[] = { *m_pVB };
		m_CommandBuffers[i].BindVertexBuffers(0, 1, PosColorVertexBuffers, offsets);

		m_CommandBuffers[i].BindIndexBuffer(*m_pIB);

		m_CommandBuffers[i].DrawIndexed(m_NumIndices);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void TriangleAndBuffers::Update()
{
	static float rotX = 0.0f;
	static float rotY = 0.0f;

	if (rotX > Bv2Pi) { rotX = 0.0f; }
	if (rotY > Bv2Pi) { rotY = 0.0f; }

	Float44 * wvp = reinterpret_cast<Float44 *>(m_pUB->GetMappedData());

	Store44(MatrixRotationY(rotX)
		* MatrixRotationX(rotY)
		* MatrixLookAtLH(VectorSet(0.0f, 0.0f, -5.0f, 1.0f), VectorSet(0.0f, 0.0f, 1.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4), &wvp->m0);

	rotX += 0.01f;
	rotY += 0.02f;
}


void TriangleAndBuffers::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}