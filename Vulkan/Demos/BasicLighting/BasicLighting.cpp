#include "BasicLighting.h"
#include "ForwardRenderer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanShader.h"
#include "VertexLayout.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "VulkanImageView.h"
#include "VulkanPipelineCache.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "Primitives.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"


BasicLighting::BasicLighting()
{
	m_AppName = "Vulkan Demo - Basic Lighting";
}


BasicLighting::~BasicLighting()
{
}


void BasicLighting::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\BasicLighting");

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


void BasicLighting::Destroy()
{
	m_pPipeline->Destroy(); BV_SAFE_DELETE(m_pPipeline);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayout->Destroy(); BV_SAFE_DELETE(m_pPipelineLayout);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	BV_SAFE_DELETE(m_pDescriptorSet);
	m_pDescriptorSetLayout->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayout);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	m_pDynamicUB->Destroy(); BV_SAFE_DELETE(m_pDynamicUB);
	m_pLightsUB->Destroy(); BV_SAFE_DELETE(m_pLightsUB);
	m_pVB->Destroy(); BV_SAFE_DELETE(m_pVB);
	m_pIB->Destroy(); BV_SAFE_DELETE(m_pIB);

	VulkanBase::Destroy();
}


void BasicLighting::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void BasicLighting::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void BasicLighting::CreateBuffers()
{
	BvVertexList vertices;
	BvIndexList<uint32_t> indices;

	CreateGeosphere(BV_VERTEX_POSITION | BV_VERTEX_NORMAL, vertices, indices, 3);
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

	m_pDevice->GetGraphicsQueue()->Submit(cb, VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence);

	fence.Wait();

	tmpVB.Destroy();
	tmpIB.Destroy();
	fence.Destroy();

	m_pCommandPool->Free(&cb);
}


void BasicLighting::CreateUniformBuffers()
{
	const auto & gpu = m_pDevice->GetGPUInfo();
	m_BufferAlignment = static_cast<uint32_t>(gpu.m_DeviceProperties.limits.minUniformBufferOffsetAlignment);
	uint32_t bufferSize = static_cast<uint32_t>(sizeof(Float44));
	if (bufferSize > m_BufferAlignment)
	{
		m_BufferAlignment = bufferSize + (m_BufferAlignment - ((m_BufferAlignment - 1) & bufferSize));
	}

	m_pDynamicUB = new VulkanBuffer(*m_pDevice);
	m_pDynamicUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_BufferAlignment * m_NumElements);
	m_pDynamicUB->Map();

	m_pLightsUB = new VulkanBuffer(*m_pDevice);
	m_pLightsUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(UBOFrameData));
	m_pLightsUB->Map();
}


void BasicLighting::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void BasicLighting::CreateDescriptorSetLayouts()
{
	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayout = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayout->Create(setLayoutInfo);
	}
}


void BasicLighting::CreateDescriptorSets()
{
	{
		m_pDescriptorSet = new VulkanDescriptorSet(*m_pDevice);
		m_pDescriptorPool->Allocate(m_pDescriptorSet, m_pDescriptorSetLayout);

		VkDescriptorBufferInfo bufferInfo[2]{};
		bufferInfo[0].buffer = *m_pDynamicUB;
		bufferInfo[0].range = m_BufferAlignment;
		bufferInfo[1].buffer = *m_pLightsUB;
		bufferInfo[1].range = sizeof(UBOFrameData);

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &bufferInfo[0]),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[1])
		};
		m_pDescriptorSet->Update(2, writeSets);
	}
}


void BasicLighting::CreatePipelineLayouts()
{
	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayout);
		m_pPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayout->Create(pipelineLayoutCreateInfo);
	}
}


void BasicLighting::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void BasicLighting::CreateGraphicsPipelines()
{
	auto vsShader = GetShaderFilePath("vs.spv");
	auto fsShader = GetShaderFilePath("fs.spv");

	VulkanShader vs(*m_pDevice), fs(*m_pDevice);
	vs.Create(vsShader.c_str());
	fs.Create(fsShader.c_str());

	GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	graphicsPipelineCreateInfo.AddVertexBinding(0, g_BvVertexSize * 6, VK_VERTEX_INPUT_RATE_VERTEX);
	graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	graphicsPipelineCreateInfo.AddVertexAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, g_BvVertexSize * 3);
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


void BasicLighting::RecordCommandBuffers()
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

		VkDeviceSize offsets[] = { 0 };
		VkBuffer PosColorVertexBuffers[] = { *m_pVB };
		m_CommandBuffers[i].BindVertexBuffers(0, 1, PosColorVertexBuffers, offsets);

		m_CommandBuffers[i].BindIndexBuffer(*m_pIB);

		m_CommandBuffers[i].BindPipeline(*m_pPipeline);


		for (uint32_t j = 0; j < m_NumElements; ++j)
		{
			uint32_t offset = m_BufferAlignment * j;
			VkDescriptorSet sets[] = { *m_pDescriptorSet };
			m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayout, 0, 1, sets, 1, &offset);

			m_CommandBuffers[i].DrawIndexed(m_NumIndices);
		}

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void BasicLighting::Update()
{
	BvFastMat viewProj = MatrixLookAtLH(VectorSet(0.0f, 0.0f, -8.0f, 1.0f), VectorSet(0.0f, 0.0f, 1.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4);

	struct UBO
	{
		Float44 w;
		Float44 wvp;
	};

	union
	{
		UBO * pAsUBData;
		char * pAsChar;
	} pData;

	pData.pAsChar = reinterpret_cast<char *>(m_pDynamicUB->GetMappedData());

	float x = -2.5f;
	float y = 1.5f;
	for (uint32_t i = 0; i < m_NumElements; ++i)
	{
		Store44(MatrixTranslation(x, y, 0.0f), pData.pAsUBData->w.m);
		Store44(Load44(pData.pAsUBData->w.m) * viewProj, pData.pAsUBData->wvp.m);

		pData.pAsChar += m_BufferAlignment;

		x += 2.5f;
		if (i / 3 == 0 && i % 3 == 2)
		{
			x = -2.5f;
			y = -1.5f;
		}
	}

	UBOFrameData * pLightData = reinterpret_cast<UBOFrameData *>(m_pLightsUB->GetMappedData());
	Store3(VectorSet(0.0f, 0.0f, -8.0f, 1.0f), pLightData->viewPos.v);
	
	static LightType lightType = LightType::Ambient;

	if (GetAsyncKeyState('1') & 0x8000) { lightType = LightType::Ambient; }
	if (GetAsyncKeyState('2') & 0x8000) { lightType = LightType::Directional; }
	if (GetAsyncKeyState('3') & 0x8000) { lightType = LightType::Point; }
	if (GetAsyncKeyState('4') & 0x8000) { lightType = LightType::Spot; }
	pLightData->light.m_LightType = lightType;

	if (lightType == LightType::Ambient)
	{
		pLightData->light.m_Color = Float3(0.3f, 0.3f, 0.3f);
	}
	if (lightType == LightType::Directional)
	{
		Store3(VectorNormalize(VectorSet(-1.0f, -1.0f, 1.0f)), pLightData->light.m_Direction.v);
		pLightData->light.m_Color = Float3(0.3f, 0.8f, 0.5f);
	}
	if (lightType == LightType::Point)
	{
		Store3(VectorSet(0.0f, 0.0f, -4.0f), pLightData->light.m_Position.v);
		pLightData->light.m_Range = 6.0f;
		Store3(VectorSet(0.7f, 0.2f, 0.4f), pLightData->light.m_Attenuation.v);
		pLightData->light.m_Color = Float3(0.1f, 0.5f, 0.9f);
	}
	if (lightType == LightType::Spot)
	{
		Store3(VectorSet(2.0f, 0.0f, -4.0f), pLightData->light.m_Position.v);
		Store3(VectorNormalize(VectorSet(1.0f, 0.5f, 1.0f)), pLightData->light.m_Direction.v);
		pLightData->light.m_Range = 9.0f;
		pLightData->light.m_OuterCos = cosf(BvPi * 0.4f);
		pLightData->light.m_InnerCos = cosf(BvPi * 0.25f);
		Store3(VectorSet(0.15f, 0.2f, 0.2f), pLightData->light.m_Attenuation.v);
		pLightData->light.m_Color = Float3(0.9f, 0.2f, 0.5f);
	}
}


void BasicLighting::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(BasicLighting)