#include "DynamicCubeMapping.h"
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
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "Skybox.h"
#include "Primitives.h"
#include "VulkanBarrier.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanSemaphore.h"
#include "VulkanCommandPool.h"
#include "VulkanPipelineCache.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"


#define DCM_TEXTURE_SIZE 512


DynamicCubeMapping::DynamicCubeMapping()
{
	m_AppName = "Vulkan Demo - Dynamic Cube Mapping";
}


DynamicCubeMapping::~DynamicCubeMapping()
{
}


void DynamicCubeMapping::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\DynamicCubeMapping");

	VulkanBase::Create(info);

	CreateRenderers();
	CreateBuffers();
	CreateDCMFramebuffers();
	CreateTextures();
	CreateSamplers();
	CreateUniformBuffers();
	CreatePipelineCaches();
	CreateDescriptorPools();
	CreateDescriptorSetLayouts();
	CreateDescriptorSets();
	CreatePipelineLayouts();
	CreateGraphicsPipelines();
	CreateSyncResources();
	CreateSkybox();
	RecordCommandBuffers();
}


void DynamicCubeMapping::Destroy()
{
	BV_SAFE_DELETE(m_pDCMUB);

	BV_SAFE_DELETE(m_pColorTextureCube);
	BV_SAFE_DELETE(m_pDepthTextureCube);

	BV_SAFE_DELETE(m_pDCMRenderPass);
	BV_SAFE_DELETE(m_pDCMFramebuffer);

	BV_SAFE_DELETE(m_pDCMDescriptorSetLayout);
	BV_SAFE_DELETE(m_pDCMDescriptorSet);
	BV_SAFE_DELETE(m_pDCMPipelineLayout);
	BV_SAFE_DELETE(m_pDCMPipeline);

	BV_SAFE_DELETE(m_pDCMFence);
	BV_SAFE_DELETE(m_pDCMSemaphore);

	m_pPipeline->Destroy(); BV_SAFE_DELETE(m_pPipeline);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayout->Destroy(); BV_SAFE_DELETE(m_pPipelineLayout);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	BV_SAFE_DELETE(m_pDescriptorSet);
	m_pDescriptorSetLayout->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayout);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	BV_SAFE_DELETE(m_pSkySampler);
	BV_SAFE_DELETE(m_pSkyTexture);
	BV_SAFE_DELETE(m_pMarbleSampler);
	BV_SAFE_DELETE(m_pMarbleTexture);

	m_pSkybox->Destroy(); BV_SAFE_DELETE(m_pSkybox);

	m_pUBWVP->Destroy(); BV_SAFE_DELETE(m_pUBWVP);
	m_pUBCam->Destroy(); BV_SAFE_DELETE(m_pUBCam);
	m_pSphereVB->Destroy(); BV_SAFE_DELETE(m_pSphereVB);
	m_pSphereIB->Destroy(); BV_SAFE_DELETE(m_pSphereIB);
	m_pCubeVB->Destroy(); BV_SAFE_DELETE(m_pCubeVB);
	m_pCubeIB->Destroy(); BV_SAFE_DELETE(m_pCubeIB);

	VulkanBase::Destroy();
}


void DynamicCubeMapping::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void DynamicCubeMapping::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void DynamicCubeMapping::CreateBuffers()
{
	{
		BvVertexList vertices;
		BvIndexList<uint32_t> indices;

		BvVertexLayout layouts[] =
		{
			BV_VERTEX_POSITION,
			BV_VERTEX_NORMAL
		};

		CreateGeosphere(BV_VERTEX_POSITION | BV_VERTEX_NORMAL, vertices, indices, 3);
		m_SphereIndices = indices.size();

		VulkanBuffer tmpVB(*m_pDevice);
		VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
		tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vbSize);
		tmpVB.Map();
		tmpVB.Update(vertices.data(), vbSize);
		tmpVB.Unmap();

		m_pSphereVB = new VulkanBuffer(*m_pDevice);
		m_pSphereVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


		VulkanBuffer tmpIB(*m_pDevice);
		VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
		tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			ibSize);
		tmpIB.Map();
		tmpIB.Update(indices.data(), ibSize);
		tmpIB.Unmap();

		m_pSphereIB = new VulkanBuffer(*m_pDevice);
		m_pSphereIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

		VulkanFence fence(*m_pDevice);
		fence.Create();

		VulkanCommandBuffer cb;
		m_pCommandPool->Allocate(&cb);

		cb.BeginCommandBuffer();
		cb.CopyBuffer(tmpVB, *m_pSphereVB, vbSize);
		cb.CopyBuffer(tmpIB, *m_pSphereIB, ibSize);
		cb.EndCommandBuffer();

		m_pDevice->GetGraphicsQueue()->Submit(cb.Get(), VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence.Get());

		fence.Wait();

		tmpVB.Destroy();
		tmpIB.Destroy();
		fence.Destroy();

		m_pCommandPool->Free(&cb);
	}

	{
		BvVertexList vertices;
		BvIndexList<uint32_t> indices;

		BvVertexLayout layouts[] =
		{
			BV_VERTEX_POSITION,
			BV_VERTEX_TEXCOORD
		};

		//CreateGeosphere(formats, 2, vertices, indices, 3);
		CreateBox(BV_VERTEX_POSITION | BV_VERTEX_TEXCOORD, vertices, indices, 2.0f, 2.0f, 2.0f);
		m_CubeIndices = indices.size();

		VulkanDevice * pDevice = g_pVulkanBase->GetDevice();

		VulkanBuffer tmpVB(*pDevice);
		VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
		tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vbSize);
		tmpVB.Map();
		tmpVB.Update(vertices.data(), vbSize);
		tmpVB.Unmap();

		m_pCubeVB = new VulkanBuffer(*pDevice);
		m_pCubeVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


		VulkanBuffer tmpIB(*pDevice);
		VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
		tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			ibSize);
		tmpIB.Map();
		tmpIB.Update(indices.data(), ibSize);
		tmpIB.Unmap();

		m_pCubeIB = new VulkanBuffer(*pDevice);
		m_pCubeIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

		VulkanFence fence(*pDevice);
		fence.Create();

		auto pPool = g_pVulkanBase->GetDefaultCommandPool();
		VulkanCommandBuffer cb;
		pPool->Allocate(&cb);

		cb.BeginCommandBuffer();
		cb.CopyBuffer(tmpVB.Get(), m_pCubeVB->Get(), vbSize);
		cb.CopyBuffer(tmpIB.Get(), m_pCubeIB->Get(), ibSize);
		cb.EndCommandBuffer();

		pDevice->GetGraphicsQueue()->Submit(cb.Get(), VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence.Get());

		fence.Wait();

		tmpVB.Destroy();
		tmpIB.Destroy();
		fence.Destroy();

		pPool->Free(&cb);
	}
}


void DynamicCubeMapping::CreateDCMFramebuffers()
{
	{
		TextureCreateInfo textureCreateInfo(DCM_TEXTURE_SIZE, DCM_TEXTURE_SIZE, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 1, 1, true);
		m_pColorTextureCube = new VulkanTexture();
		m_pColorTextureCube->Create(textureCreateInfo);
	}

	VkFormat depthFormat = m_pDevice->GetBestDepthFormat();
	{
		TextureCreateInfo textureCreateInfo(DCM_TEXTURE_SIZE, DCM_TEXTURE_SIZE, depthFormat,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, 1, true);
		m_pDepthTextureCube = new VulkanTexture();
		m_pDepthTextureCube->Create(textureCreateInfo);
	}

	AttachmentReference colorRef(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	AttachmentReference depthRef(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.AddAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	renderPassCreateInfo.AddAttachment(depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	renderPassCreateInfo.AddSubpass(1, &colorRef, &depthRef);
	renderPassCreateInfo.AddDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	renderPassCreateInfo.AddDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);

	m_pDCMRenderPass = new VulkanRenderPass(*m_pDevice);
	m_pDCMRenderPass->Create(renderPassCreateInfo);

	VkImageView views[] = { *m_pColorTextureCube->GetView(), *m_pDepthTextureCube->GetView() };
	FramebufferCreateInfo framebufferCreateInfo(m_pDCMRenderPass->Get(), 2, views, DCM_TEXTURE_SIZE, DCM_TEXTURE_SIZE, 6);
	m_pDCMFramebuffer = new VulkanFramebuffer(*m_pDevice);
	m_pDCMFramebuffer->Create(framebufferCreateInfo);
}


void DynamicCubeMapping::CreateTextures()
{
	auto purpleValley = GetTextureFilePath("purplevalley_cube.dds");
	auto whiteMarble = GetTextureFilePath("white_marble.dds");

	m_pSkyTexture = new VulkanTexture();
	m_pSkyTexture->CreateFromFile(purpleValley.c_str());

	m_pMarbleTexture = new VulkanTexture();
	m_pMarbleTexture->CreateFromFile(whiteMarble.c_str());
}


void DynamicCubeMapping::CreateSamplers()
{
	SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.SetAnisotropy(VK_TRUE, 16.0f);
	samplerCreateInfo.maxLod = static_cast<float>(m_pSkyTexture->GetLevelCount());
	m_pSkySampler = new VulkanSampler(*m_pDevice);
	m_pSkySampler->Create(samplerCreateInfo);

	samplerCreateInfo.maxLod = static_cast<float>(m_pMarbleTexture->GetLevelCount());
	m_pMarbleSampler = new VulkanSampler(*m_pDevice);
	m_pMarbleSampler->Create(samplerCreateInfo);
}


void DynamicCubeMapping::CreateSkybox()
{
	m_pSkybox = new Skybox();
	m_pSkybox->Create(*m_pDescriptorPool, *m_pPipelineCache, *m_pForwardRenderer->GetRenderPass());
	m_pSkybox->Set(*m_pSkyTexture, *m_pSkySampler);
}


void DynamicCubeMapping::CreateUniformBuffers()
{
	m_pUBWVP = new VulkanBuffer(*m_pDevice);
	m_pUBWVP->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(WVP));
	m_pUBWVP->Map();

	m_pUBCam = new VulkanBuffer(*m_pDevice);
	m_pUBCam->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float4));
	m_pUBCam->Map();

	m_pDCMUB = new VulkanBuffer(*m_pDevice);
	m_pDCMUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float44) * 6);
	m_pDCMUB->Map();
}


void DynamicCubeMapping::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(3);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 6);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void DynamicCubeMapping::CreateDescriptorSetLayouts()
{
	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayout = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayout->Create(setLayoutInfo);
	}

	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDCMDescriptorSetLayout = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDCMDescriptorSetLayout->Create(setLayoutInfo);
	}
}


void DynamicCubeMapping::CreateDescriptorSets()
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
		imageInfo.imageView = *m_pColorTextureCube->GetView();
		imageInfo.sampler = *m_pMarbleSampler;

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[0]),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[1]),
			WriteDescriptorSet(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
		};
		m_pDescriptorSet->Update(3, writeSets);
	}

	{
		m_pDCMDescriptorSet = new VulkanDescriptorSet(*m_pDevice);
		m_pDescriptorPool->Allocate(m_pDCMDescriptorSet, m_pDCMDescriptorSetLayout);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_pDCMUB->Get();
		bufferInfo.range = sizeof(Float44) * 6;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_pMarbleTexture->GetView()->Get();
		imageInfo.sampler = m_pMarbleSampler->Get();

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
		};
		m_pDCMDescriptorSet->Update(2, writeSets);
	}
}


void DynamicCubeMapping::CreatePipelineLayouts()
{
	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayout);
		m_pPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayout->Create(pipelineLayoutCreateInfo);
	}

	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(m_pDCMDescriptorSetLayout->Get());
		m_pDCMPipelineLayout = new VulkanPipelineLayout(*m_pDevice);
		m_pDCMPipelineLayout->Create(pipelineLayoutCreateInfo);
	}
}


void DynamicCubeMapping::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void DynamicCubeMapping::CreateGraphicsPipelines()
{
	{
		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		graphicsPipelineCreateInfo.AddVertexBinding(0, sizeof(BvVertexPack) * 6, VK_VERTEX_INPUT_RATE_VERTEX);
		graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
		graphicsPipelineCreateInfo.AddVertexAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(BvVertexPack) * 3);
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

	{
		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		graphicsPipelineCreateInfo.AddVertexBinding(0, g_BvVertexSize * 5, VK_VERTEX_INPUT_RATE_VERTEX);
		graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
		graphicsPipelineCreateInfo.AddVertexAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, g_BvVertexSize * 3);
		// No tessellation
		// Default rasterizer already set in constructor
		// Default depth / stencil already set in constructor
		// Default Multisampling already set in constructor
		graphicsPipelineCreateInfo.AddBlendAttachment();
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

		auto vsShader = g_pVulkanBase->GetShaderFilePath("vsDCM.spv");
		auto gsShader = g_pVulkanBase->GetShaderFilePath("gsDCM.spv");
		auto fsShader = g_pVulkanBase->GetShaderFilePath("fsDCM.spv");

		VulkanShader vs(*m_pDevice), gs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		gs.Create(gsShader.c_str());
		fs.Create(fsShader.c_str());

		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs.Get());
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_GEOMETRY_BIT, gs.Get());
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs.Get());
		graphicsPipelineCreateInfo.SetRenderInfo(m_pDCMPipelineLayout->Get(), m_pDCMRenderPass->Get(), 0);

		m_pDCMPipeline = new VulkanPipeline(*m_pDevice);
		m_pDCMPipeline->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		gs.Destroy();
		fs.Destroy();
	}
}


void DynamicCubeMapping::CreateSyncResources()
{
	m_pDCMFence = new VulkanFence(*m_pDevice);
	m_pDCMFence->Create(VK_FENCE_CREATE_SIGNALED_BIT);

	m_pDCMSemaphore = new VulkanSemaphore(*m_pDevice);
	m_pDCMSemaphore->Create();
}


void DynamicCubeMapping::RecordDCMCommandBuffers()
{
	if (!m_pDCMCommandBuffer)
	{
		m_pDCMCommandBuffer = new VulkanCommandBuffer();
		m_pCommandPool->Allocate(m_pDCMCommandBuffer);
	}

	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.3f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	m_pDCMCommandBuffer->BeginCommandBuffer();

	m_pDCMCommandBuffer->BeginRenderPass(m_pDCMRenderPass->Get(), m_pDCMFramebuffer->Get(), DCM_TEXTURE_SIZE, DCM_TEXTURE_SIZE, 2, clearValues);

	m_pDCMCommandBuffer->SetViewport(static_cast<float>(DCM_TEXTURE_SIZE), static_cast<float>(DCM_TEXTURE_SIZE));

	m_pDCMCommandBuffer->SetScissor(DCM_TEXTURE_SIZE, DCM_TEXTURE_SIZE);

	m_pDCMCommandBuffer->BindPipeline(m_pDCMPipeline->Get());

	VkDescriptorSet set[] = { m_pDCMDescriptorSet->Get() };
	m_pDCMCommandBuffer->BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_pDCMPipelineLayout->Get(), 0, 1, set);

	VkDeviceSize offsets[] = { 0 };
	VkBuffer vbs[] = { m_pCubeVB->Get() };
	m_pDCMCommandBuffer->BindVertexBuffers(0, 1, vbs, offsets);
	m_pDCMCommandBuffer->BindIndexBuffer(m_pCubeIB->Get());
	m_pDCMCommandBuffer->DrawIndexed(m_CubeIndices);

	m_pDCMCommandBuffer->EndRenderPass();

	m_pDCMCommandBuffer->EndCommandBuffer();
}


void DynamicCubeMapping::RecordCommandBuffers()
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
		VkBuffer PosColorVertexBuffers[] = { *m_pSphereVB };
		m_CommandBuffers[i].BindVertexBuffers(0, 1, PosColorVertexBuffers, offsets);

		m_CommandBuffers[i].BindIndexBuffer(*m_pSphereIB);

		m_CommandBuffers[i].BindPipeline(*m_pPipeline);

		VkDescriptorSet sets[] = { *m_pDescriptorSet };
		m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayout, 0, 1, sets);

		m_CommandBuffers[i].DrawIndexed(m_SphereIndices);

		m_pSkybox->Render(m_CommandBuffers[i]);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void DynamicCubeMapping::Update()
{
	static float rotY = 0.0f;

	if (rotY > Bv2Pi) { rotY = 0.0f; }

	WVP * wvp = reinterpret_cast<WVP *>(m_pUBWVP->GetMappedData());


	BvFastMat vp = MatrixLookAt(VectorSet(0.0f, 0.0f, -6.0f), VectorSet(0.0f, 0.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4);

	Store44(MatrixIdentity(), wvp->w.m);
	Store44(vp, wvp->vp.m);

	Float44 viewProj;
	Store44(vp, viewProj.m);


	Float3 camPos(0.0f, 0.0f, -6.0f);

	m_pUBCam->Update(&camPos, sizeof(Float3));

	m_pSkybox->Update(camPos, viewProj);


	BvFastVec dirs[6] =
	{
		VectorSet(1.0f, 0.0f, 0.0f),
		VectorSet(-1.0f, 0.0f, 0.0f),
		VectorSet(0.0f, 1.0f, 0.0f),
		VectorSet(0.0f, -1.0f, 0.0f),
		VectorSet(0.0f, 0.0f, 1.0f),
		VectorSet(0.0f, 0.0f, -1.0f)
	};

	BvFastVec ups[6] =
	{
		VectorSet(0.0f, 1.0f, 0.0f),
		VectorSet(0.0f, 1.0f, 0.0f),
		VectorSet(0.0f, 0.0f, -1.0f),
		VectorSet(0.0f, 0.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f),
		VectorSet(0.0f, 1.0f, 0.0f)
	};

	BvFastVec pos = VectorSet(0.0f, 0.0f, -6.0f);

	BvFastMat proj = MatrixPerspectiveLH_VK(0.1f, 100.0f, 1.0f, BvPiDiv2);

	BvFastMat w = MatrixTranslation(0.0f, 0.0f, -8.0f) * MatrixRotationY(rotY);
	rotY += 0.05f;

	Float44 vps[6];
	Store44(w * MatrixLookAt(pos, dirs[0], ups[0]) * proj, vps[0].m);
	Store44(w * MatrixLookAt(pos, dirs[1], ups[1]) * proj, vps[1].m);
	Store44(w * MatrixLookAt(pos, dirs[2], ups[2]) * proj, vps[2].m);
	Store44(w * MatrixLookAt(pos, dirs[3], ups[3]) * proj, vps[3].m);
	Store44(w * MatrixLookAt(pos, dirs[4], ups[4]) * proj, vps[4].m);
	Store44(w * MatrixLookAt(pos, dirs[5], ups[5]) * proj, vps[5].m);

	m_pDCMUB->Update(vps, sizeof(Float44) * 6);
}


void DynamicCubeMapping::Render()
{
	PrepareFrame();

	m_pDCMFence->Wait();
	m_pDCMFence->Reset();
	RecordDCMCommandBuffers();
	m_pDevice->GetGraphicsQueue()->Submit(*m_pDCMCommandBuffer, *m_pPresentCompleteSemaphore, *m_pDCMSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, *m_pDCMFence);

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();
	m_pDevice->GetGraphicsQueue()->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pDCMSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(DynamicCubeMapping)