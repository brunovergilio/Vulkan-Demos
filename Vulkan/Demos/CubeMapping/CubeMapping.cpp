#include "CubeMapping.h"
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
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "Skybox.h"
#include "Primitives.h"


CubeMapping::CubeMapping()
{
	m_AppName = "Vulkan Demo - Cube Mapping";
}


CubeMapping::~CubeMapping()
{
}


void CubeMapping::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\CubeMapping");

	VulkanBase::Create(info);

	CreateRenderers();
	CreateBuffers();
	CreateTextures();
	CreateSamplers();
	CreateUniformBuffers();
	CreateDescriptorPools();
	CreateDescriptorSetLayouts();
	CreateDescriptorSets();
	CreatePipelineLayouts();
	CreatePipelineCaches();
	CreateGraphicsPipelines();
	CreateSkybox();
	RecordCommandBuffers();
}


void CubeMapping::Destroy()
{
	m_pPipeline->Destroy(); BV_SAFE_DELETE(m_pPipeline);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayout->Destroy(); BV_SAFE_DELETE(m_pPipelineLayout);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	BV_SAFE_DELETE(m_pDescriptorSet);
	m_pDescriptorSetLayout->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayout);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	m_pSkySampler->Destroy();
	m_pSkyTexture->Destroy();

	m_pSkybox->Destroy();

	m_pUBWVP->Destroy(); BV_SAFE_DELETE(m_pUBWVP);
	m_pUBCam->Destroy(); BV_SAFE_DELETE(m_pUBCam);
	m_pVB->Destroy(); BV_SAFE_DELETE(m_pVB);
	m_pIB->Destroy(); BV_SAFE_DELETE(m_pIB);

	VulkanBase::Destroy();
}


void CubeMapping::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void CubeMapping::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void CubeMapping::CreateBuffers()
{
	BvVertexList vertices;
	BvIndexList<uint32_t> indices;

	BvVertexLayout layouts[] =
	{
		BV_VERTEX_POSITION,
		BV_VERTEX_NORMAL
	};

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


void CubeMapping::CreateTextures()
{
	std::string texName = GetTextureFilePath("purplevalley_cube.dds");
	m_pSkyTexture = new VulkanTexture();
	m_pSkyTexture->CreateFromFile(texName.c_str());
}


void CubeMapping::CreateSamplers()
{
	SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.SetAnisotropy(VK_TRUE, static_cast<float>(m_pSkyTexture->GetLevelCount()));
	samplerCreateInfo.maxLod = static_cast<float>(m_pSkyTexture->GetLevelCount());
	m_pSkySampler = new VulkanSampler(*m_pDevice);
	m_pSkySampler->Create(samplerCreateInfo);
}


void CubeMapping::CreateSkybox()
{
	m_pSkybox = new Skybox();
	m_pSkybox->Create(*m_pDescriptorPool, *m_pPipelineCache, *m_pForwardRenderer->GetRenderPass());
	m_pSkybox->Set(*m_pSkyTexture, *m_pSkySampler);
}


void CubeMapping::CreateUniformBuffers()
{
	m_pUBWVP = new VulkanBuffer(*m_pDevice);
	m_pUBWVP->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(WVP));
	m_pUBWVP->Map();

	m_pUBCam = new VulkanBuffer(*m_pDevice);
	m_pUBCam->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float4));
	m_pUBCam->Map();
}


void CubeMapping::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(2);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void CubeMapping::CreateDescriptorSetLayouts()
{
	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayout = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayout->Create(setLayoutInfo);
	}
}


void CubeMapping::CreateDescriptorSets()
{
	{
		m_pDescriptorSet = new VulkanDescriptorSet(*m_pDevice);
		m_pDescriptorPool->Allocate(m_pDescriptorSet, m_pDescriptorSetLayout);

		VkDescriptorBufferInfo bufferInfo[2]{};
		bufferInfo[0].buffer = *m_pUBWVP;
		bufferInfo[0].range = sizeof(WVP);
		bufferInfo[1].buffer = *m_pUBCam;
		bufferInfo[1].range = sizeof(Float4);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *m_pSkyTexture->GetView();
		imageInfo.sampler = *m_pSkySampler;

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[0]),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[1]),
			WriteDescriptorSet(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
		};
		m_pDescriptorSet->Update(3, writeSets);
	}
}


void CubeMapping::CreatePipelineLayouts()
{
	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayout);
		m_pPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayout->Create(pipelineLayoutCreateInfo);
	}
}


void CubeMapping::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void CubeMapping::CreateGraphicsPipelines()
{
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

	auto vsShader = GetShaderFilePath("vsReflect.spv");
	auto fsShader = GetShaderFilePath("fsReflect.spv");

	VulkanShader vs(*m_pDevice), fs(*m_pDevice);
	vs.Create(vsShader.c_str());
	fs.Create(fsShader.c_str());

	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
	graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayout, *m_pForwardRenderer->GetRenderPass(), 0);

	m_pPipeline = new VulkanPipeline(*m_pDevice);
	m_pPipeline->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

	vs.Destroy();
	fs.Destroy();
}


void CubeMapping::RecordCommandBuffers()
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

		{
			VkDescriptorSet sets[] = { *m_pDescriptorSet };
			m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayout, 0, 1, sets);
		}

		m_CommandBuffers[i].DrawIndexed(m_NumIndices);

		m_pSkybox->Render(m_CommandBuffers[i]);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void CubeMapping::Update()
{
	//static float rotY = 0.0f;

	//if (rotY > Bv2Pi) { rotY = 0.0f; }

	WVP * wvp = reinterpret_cast<WVP *>(m_pUBWVP->GetMappedData());


	BvFastMat vp = MatrixLookAt(VectorSet(0.0f, 0.0f, -6.0f), VectorSet(0.0f, 0.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4);

	Store44(MatrixIdentity(), wvp->w.m);
	Store44(vp, wvp->vp.m);

	//rotY += 0.003f;

	Float44 viewProj;
	Store44(vp, viewProj.m);


	Float3 camPos(0.0f, 0.0f, -6.0f);

	m_pUBCam->Update(&camPos, sizeof(Float3));

	m_pSkybox->Update(camPos, viewProj);
}


void CubeMapping::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pDevice->GetGraphicsQueue()->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(CubeMapping)