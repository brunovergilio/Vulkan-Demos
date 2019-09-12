#include "DeferredRenderingRenderPass.h"
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
#include "VulkanImage.h"


DeferredRenderingRenderPass::DeferredRenderingRenderPass()
{
	m_AppName = "Vulkan Demo - Deferred Rendering Render Pass";
}


DeferredRenderingRenderPass::~DeferredRenderingRenderPass()
{
}


void DeferredRenderingRenderPass::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\DeferredRenderingRenderPass");

	VulkanBase::Create(info);

	CreateRenderers();
	CreateBuffers();
	CreateUniformBuffers();
	CreateTextures();
	CreateDescriptorPools();
	CreateDescriptorSetLayouts();
	CreateDescriptorSets();
	CreatePipelineLayouts();
	CreatePipelineCaches();
	CreateGraphicsPipelines();
	CreateSyncResources();
	//RecordCommandBuffers();

	m_pCommandPool->Allocate(&m_DeferredCommandBuffer);
}


void DeferredRenderingRenderPass::Destroy()
{
	m_pDepthTex->Destroy(); BV_SAFE_DELETE(m_pDepthTex);
	m_pDiffuseTex->Destroy(); BV_SAFE_DELETE(m_pDiffuseTex);
	m_pNormalTex->Destroy(); BV_SAFE_DELETE(m_pNormalTex);
	m_pDepthReadOnlyView->Destroy(); BV_SAFE_DELETE(m_pDepthReadOnlyView);

	m_pTexture->Destroy(); BV_SAFE_DELETE(m_pTexture);
	m_pSampler->Destroy(); BV_SAFE_DELETE(m_pSampler);
	m_pGBufferSampler->Destroy(); BV_SAFE_DELETE(m_pGBufferSampler);

	m_pPipelineGBuffer->Destroy(); BV_SAFE_DELETE(m_pPipelineGBuffer);
	m_pPipelineDebug->Destroy(); BV_SAFE_DELETE(m_pPipelineDebug);
	m_pPipelineLight->Destroy(); BV_SAFE_DELETE(m_pPipelineLight);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayoutGBuffer->Destroy(); BV_SAFE_DELETE(m_pPipelineLayoutGBuffer);
	m_pPipelineLayoutDebug->Destroy(); BV_SAFE_DELETE(m_pPipelineLayoutDebug);
	m_pPipelineLayoutLight->Destroy(); BV_SAFE_DELETE(m_pPipelineLayoutLight);

	m_pDeferredFramebuffer->Destroy(); BV_SAFE_DELETE(m_pDeferredFramebuffer);
	for (auto && framebuffer : m_CompositeFramebuffers) { framebuffer.Destroy(); }
	m_pDeferredRenderPass->Destroy(); BV_SAFE_DELETE(m_pDeferredRenderPass);
	m_pCompositeRenderPass->Destroy(); BV_SAFE_DELETE(m_pCompositeRenderPass);

	BV_SAFE_DELETE(m_pDescriptorSetGBuffer);
	BV_SAFE_DELETE(m_pDescriptorSetDebug);
	m_pDescriptorSetLayoutGBuffer->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayoutGBuffer);
	m_pDescriptorSetLayoutDebug->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayoutDebug);
	m_pDescriptorSetLayoutLight->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayoutLight);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	m_pUnpackGBufferUB->Destroy(); BV_SAFE_DELETE(m_pUnpackGBufferUB);
	m_pDynamicUB->Destroy(); BV_SAFE_DELETE(m_pDynamicUB);
	m_pDebugUB->Destroy(); BV_SAFE_DELETE(m_pDebugUB);
	m_pLightsUB->Destroy(); BV_SAFE_DELETE(m_pLightsUB);
	m_pGridVB->Destroy(); BV_SAFE_DELETE(m_pGridVB);
	m_pGridIB->Destroy(); BV_SAFE_DELETE(m_pGridIB);
	m_pBoxVB->Destroy(); BV_SAFE_DELETE(m_pBoxVB);
	m_pBoxIB->Destroy(); BV_SAFE_DELETE(m_pBoxIB);

	m_pDeferredCompleteSemaphore->Destroy(); BV_SAFE_DELETE(m_pDeferredCompleteSemaphore);
	m_pDeferredFence->Destroy(); BV_SAFE_DELETE(m_pDeferredFence);

	VulkanBase::Destroy();
}


void DeferredRenderingRenderPass::Resize()
{
	VulkanBase::Resize();

	m_pDepthReadOnlyView->Destroy();
	m_pDepthTex->Destroy();
	m_pDiffuseTex->Destroy();
	m_pNormalTex->Destroy();
	m_pDeferredFramebuffer->Destroy();
	for (auto && framebuffer : m_CompositeFramebuffers)
	{
		framebuffer.Destroy();
	}

	CreateRenderers();
	CreateDescriptorSets();

	//RecordCommandBuffers();
}


void DeferredRenderingRenderPass::CreateRenderers()
{
	{
		TextureCreateInfo textureCreateInfo;
		textureCreateInfo.SetDimensions(m_Width, m_Height);
		textureCreateInfo.SetFormat(VK_FORMAT_D32_SFLOAT_S8_UINT);
		textureCreateInfo.UseForDepthStencil().UseForShaderResource();

		if (!m_pDepthTex) { m_pDepthTex = new VulkanTexture(); }
		m_pDepthTex->Create(textureCreateInfo);

		ImageViewCreateInfo imageViewCreateInfo(*m_pDepthTex->GetImage(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_ASPECT_DEPTH_BIT);
		m_pDepthReadOnlyView = new VulkanImageView(*m_pDevice);
		m_pDepthReadOnlyView->Create(imageViewCreateInfo);
	}
	{
		TextureCreateInfo textureCreateInfo;
		textureCreateInfo.SetDimensions(m_Width, m_Height);
		textureCreateInfo.SetFormat(VK_FORMAT_R8G8B8A8_UNORM);
		textureCreateInfo.UseForColor().UseForShaderResource();

		if (!m_pDiffuseTex) { m_pDiffuseTex = new VulkanTexture(); }
		m_pDiffuseTex->Create(textureCreateInfo);
	}
	{
		TextureCreateInfo textureCreateInfo;
		textureCreateInfo.SetDimensions(m_Width, m_Height);
		textureCreateInfo.SetFormat(VK_FORMAT_R16G16B16A16_UNORM);
		textureCreateInfo.UseForColor().UseForShaderResource();

		if (!m_pNormalTex) { m_pNormalTex = new VulkanTexture(); }
		m_pNormalTex->Create(textureCreateInfo);
	}

	if (!m_pDeferredRenderPass)
	{
		RenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.AddAttachment(VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		renderPassCreateInfo.AddAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		renderPassCreateInfo.AddAttachment(VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		renderPassCreateInfo.m_Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassCreateInfo.m_Attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

		AttachmentReference depthRef1(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		AttachmentReference colorRefs[] =
		{
			AttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
			AttachmentReference(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
		};

		renderPassCreateInfo.AddSubpass(2, colorRefs, &depthRef1);

		renderPassCreateInfo.AddDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		renderPassCreateInfo.AddDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);

		m_pDeferredRenderPass = new VulkanRenderPass(*m_pDevice);
		m_pDeferredRenderPass->Create(renderPassCreateInfo);
	}


	{
		VkImageView views[] = { *m_pDepthTex->GetView(), *m_pDiffuseTex->GetView(), *m_pNormalTex->GetView() };
		FramebufferCreateInfo frameBufferCreateInfo(*m_pDeferredRenderPass, 3, views, m_Width, m_Height);
		if (!m_pDeferredFramebuffer) { m_pDeferredFramebuffer = new VulkanFramebuffer(*m_pDevice); }
		m_pDeferredFramebuffer->Create(frameBufferCreateInfo);
	}


	if (!m_pCompositeRenderPass)
	{
		AttachmentReference depthRef(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		AttachmentReference colorRef(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		RenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.AddAttachment(VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		renderPassCreateInfo.AddAttachment(m_pSwapchain->GetColorFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		renderPassCreateInfo.m_Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		renderPassCreateInfo.m_Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

		renderPassCreateInfo.AddSubpass(1, &colorRef, &depthRef);
		renderPassCreateInfo.AddDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		renderPassCreateInfo.AddDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);

		m_pCompositeRenderPass = new VulkanRenderPass(*m_pDevice);
		m_pCompositeRenderPass->Create(renderPassCreateInfo);
	}

	m_CompositeFramebuffers.resize(m_pSwapchain->GetImageCount(), VulkanFramebuffer(*m_pDevice));
	for (auto i = 0U; i < m_CompositeFramebuffers.size(); ++i)
	{
		VkImageView views[] = { *m_pDepthTex->GetView(), m_pSwapchain->GetViews()[i] };
		FramebufferCreateInfo frameBufferCreateInfo(*m_pCompositeRenderPass, 2, views, m_Width, m_Height);
		m_CompositeFramebuffers[i].Create(frameBufferCreateInfo);
	}
}


void DeferredRenderingRenderPass::CreateBuffers()
{
	{
		BvVertexList vertices;
		BvIndexList<uint32_t> indices;

		CreateGeosphere(BV_VERTEX_POSITION | BV_VERTEX_NORMAL | BV_VERTEX_TEXCOORD, vertices, indices);
		m_NumIndicesBox = indices.size();

		VulkanBuffer tmpVB(*m_pDevice);
		VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
		tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vbSize);
		tmpVB.Map();
		tmpVB.Update(vertices.data(), vbSize);
		tmpVB.Unmap();

		m_pBoxVB = new VulkanBuffer(*m_pDevice);
		m_pBoxVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


		VulkanBuffer tmpIB(*m_pDevice);
		VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
		tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			ibSize);
		tmpIB.Map();
		tmpIB.Update(indices.data(), ibSize);
		tmpIB.Unmap();

		m_pBoxIB = new VulkanBuffer(*m_pDevice);
		m_pBoxIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

		VulkanFence fence(*m_pDevice);
		fence.Create();

		VulkanCommandBuffer cb;
		m_pCommandPool->Allocate(&cb);

		cb.BeginCommandBuffer();
		cb.CopyBuffer(tmpVB, *m_pBoxVB, vbSize);
		cb.CopyBuffer(tmpIB, *m_pBoxIB, ibSize);
		cb.EndCommandBuffer();

		m_pDevice->GetGraphicsQueue()->Submit(cb, VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence);

		fence.Wait();

		tmpVB.Destroy();
		tmpIB.Destroy();
		fence.Destroy();

		m_pCommandPool->Free(&cb);
	}

	{
		BvVertexList vertices;
		BvIndexList<uint32_t> indices;

		CreateGrid(BV_VERTEX_POSITION | BV_VERTEX_NORMAL | BV_VERTEX_TEXCOORD, vertices, indices, 9.0f, 9.0f);
		m_NumIndicesGrid = indices.size();

		VulkanBuffer tmpVB(*m_pDevice);
		VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
		tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vbSize);
		tmpVB.Map();
		tmpVB.Update(vertices.data(), vbSize);
		tmpVB.Unmap();

		m_pGridVB = new VulkanBuffer(*m_pDevice);
		m_pGridVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


		VulkanBuffer tmpIB(*m_pDevice);
		VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
		tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			ibSize);
		tmpIB.Map();
		tmpIB.Update(indices.data(), ibSize);
		tmpIB.Unmap();

		m_pGridIB = new VulkanBuffer(*m_pDevice);
		m_pGridIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

		VulkanFence fence(*m_pDevice);
		fence.Create();

		VulkanCommandBuffer cb;
		m_pCommandPool->Allocate(&cb);

		cb.BeginCommandBuffer();
		cb.CopyBuffer(tmpVB, *m_pGridVB, vbSize);
		cb.CopyBuffer(tmpIB, *m_pGridIB, ibSize);
		cb.EndCommandBuffer();

		m_pDevice->GetGraphicsQueue()->Submit(cb, VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence);

		fence.Wait();

		tmpVB.Destroy();
		tmpIB.Destroy();
		fence.Destroy();

		m_pCommandPool->Free(&cb);
	}
}


void DeferredRenderingRenderPass::CreateUniformBuffers()
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

	m_pDebugUB = new VulkanBuffer(*m_pDevice);
	m_pDebugUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(DebugData));
	m_pDebugUB->Map();

	m_pUnpackGBufferUB = new VulkanBuffer(*m_pDevice);
	m_pUnpackGBufferUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(GBufferUnpackData));
	m_pUnpackGBufferUB->Map();
}

void DeferredRenderingRenderPass::CreateTextures()
{
	auto tex = GetTextureFilePath("metal.dds");
	m_pTexture = new VulkanTexture();
	m_pTexture->CreateFromFile(tex.c_str());

	SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.SetAnisotropy(VK_TRUE, 16.0f);
	samplerCreateInfo.maxLod = static_cast<float>(m_pTexture->GetLevelCount());

	m_pSampler = new VulkanSampler(*m_pDevice);
	m_pSampler->Create(samplerCreateInfo);

	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.minFilter = samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	m_pGBufferSampler = new VulkanSampler(*m_pDevice);
	m_pGBufferSampler->Create(samplerCreateInfo);
}


void DeferredRenderingRenderPass::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(3);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void DeferredRenderingRenderPass::CreateDescriptorSetLayouts()
{
	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayoutGBuffer = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayoutGBuffer->Create(setLayoutInfo);
	}

	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayoutDebug = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayoutDebug->Create(setLayoutInfo);
	}

	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayoutLight = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayoutLight->Create(setLayoutInfo);
	}
}


void DeferredRenderingRenderPass::CreateDescriptorSets()
{
	{
		if (!m_pDescriptorSetGBuffer)
		{
			m_pDescriptorSetGBuffer = new VulkanDescriptorSet(*m_pDevice);
			m_pDescriptorPool->Allocate(m_pDescriptorSetGBuffer, m_pDescriptorSetLayoutGBuffer);
		}

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = *m_pDynamicUB;
		bufferInfo.range = m_BufferAlignment;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *m_pTexture->GetView();
		imageInfo.sampler = *m_pSampler;

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &bufferInfo),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
		};
		m_pDescriptorSetGBuffer->Update(2, writeSets);
	}

	{
		if (!m_pDescriptorSetDebug)
		{
			m_pDescriptorSetDebug = new VulkanDescriptorSet(*m_pDevice);
			m_pDescriptorPool->Allocate(m_pDescriptorSetDebug, m_pDescriptorSetLayoutDebug);
		}

		VkDescriptorImageInfo imageInfo[3]{};
		imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		imageInfo[0].imageView = *m_pDepthReadOnlyView;
		imageInfo[0].sampler = *m_pGBufferSampler;
		imageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[1].imageView = *m_pDiffuseTex->GetView();
		imageInfo[1].sampler = *m_pGBufferSampler;
		imageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[2].imageView = *m_pNormalTex->GetView();
		imageInfo[2].sampler = *m_pGBufferSampler;

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = *m_pDebugUB;
		bufferInfo.range = sizeof(DebugData);

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[0]),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[1]),
			WriteDescriptorSet(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[2]),
			WriteDescriptorSet(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo)
		};
		m_pDescriptorSetDebug->Update(4, writeSets);
	}

	{
		if (!m_pDescriptorSetLight)
		{
			m_pDescriptorSetLight = new VulkanDescriptorSet(*m_pDevice);
			m_pDescriptorPool->Allocate(m_pDescriptorSetLight, m_pDescriptorSetLayoutLight);
		}

		VkDescriptorImageInfo imageInfo[3]{};
		imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		imageInfo[0].imageView = *m_pDepthReadOnlyView;
		imageInfo[0].sampler = *m_pGBufferSampler;
		imageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[1].imageView = *m_pDiffuseTex->GetView();
		imageInfo[1].sampler = *m_pGBufferSampler;
		imageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[2].imageView = *m_pNormalTex->GetView();
		imageInfo[2].sampler = *m_pGBufferSampler;

		VkDescriptorBufferInfo bufferInfo[2]{};
		bufferInfo[0].buffer = *m_pUnpackGBufferUB;
		bufferInfo[0].range = sizeof(GBufferUnpackData);
		bufferInfo[1].buffer = *m_pLightsUB;
		bufferInfo[1].range = sizeof(UBOFrameData);

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[0]),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[1]),
			WriteDescriptorSet(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo[2]),
			WriteDescriptorSet(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[0]),
			WriteDescriptorSet(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo[1])
		};
		m_pDescriptorSetLight->Update(5, writeSets);
	}
}


void DeferredRenderingRenderPass::CreatePipelineLayouts()
{
	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayoutGBuffer);
		m_pPipelineLayoutGBuffer = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayoutGBuffer->Create(pipelineLayoutCreateInfo);
	}

	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayoutDebug);
		m_pPipelineLayoutDebug = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayoutDebug->Create(pipelineLayoutCreateInfo);
	}

	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayoutLight);
		m_pPipelineLayoutLight = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayoutLight->Create(pipelineLayoutCreateInfo);
	}
}


void DeferredRenderingRenderPass::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void DeferredRenderingRenderPass::CreateGraphicsPipelines()
{
	{
		auto vsShader = GetShaderFilePath("vsGBuffer.spv");
		auto fsShader = GetShaderFilePath("fsGBuffer.spv");

		VulkanShader vs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		fs.Create(fsShader.c_str());

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		graphicsPipelineCreateInfo.AddVertexBinding(0, g_BvVertexSize * 8, VK_VERTEX_INPUT_RATE_VERTEX);
		graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
		graphicsPipelineCreateInfo.AddVertexAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, g_BvVertexSize * 3);
		graphicsPipelineCreateInfo.AddVertexAttribute(2, 0, VK_FORMAT_R32G32B32_SFLOAT, g_BvVertexSize * 5);
		// No tessellation
		// Default rasterizer already set in constructor
		// Default depth already set in constructor
		graphicsPipelineCreateInfo.SetStencilTest(VK_TRUE);
		graphicsPipelineCreateInfo.SetStencilFunc(VK_COMPARE_OP_ALWAYS, 1);
		graphicsPipelineCreateInfo.SetStencilOp(VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_STENCIL_OP_KEEP);
		// Default Multisampling already set in constructor
		graphicsPipelineCreateInfo.AddBlendAttachment();
		graphicsPipelineCreateInfo.AddBlendAttachment();
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
		graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayoutGBuffer, *m_pDeferredRenderPass, 0);

		m_pPipelineGBuffer = new VulkanPipeline(*m_pDevice);
		m_pPipelineGBuffer->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		fs.Destroy();
	}

	{
		auto vsShader = GetShaderFilePath("vsDebug.spv");
		auto fsShader = GetShaderFilePath("fsDebug.spv");

		VulkanShader vs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		fs.Create(fsShader.c_str());

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		// No tessellation
		// Default rasterizer already set in constructor
		graphicsPipelineCreateInfo.SetDepthTest(VK_FALSE, VK_FALSE);
		// Default stencil already set in constructor
		// Default Multisampling already set in constructor
		graphicsPipelineCreateInfo.AddBlendAttachment();
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
		graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayoutDebug, *m_pCompositeRenderPass);

		m_pPipelineDebug = new VulkanPipeline(*m_pDevice);
		m_pPipelineDebug->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		fs.Destroy();
	}

	{
		auto vsShader = GetShaderFilePath("vsLight.spv");
		auto fsShader = GetShaderFilePath("fsLight.spv");

		VulkanShader vs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		fs.Create(fsShader.c_str());

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		// No tessellation
		// Default rasterizer already set in constructor
		graphicsPipelineCreateInfo.SetDepthTest(VK_TRUE, VK_FALSE);
		graphicsPipelineCreateInfo.SetStencilTest(VK_TRUE);
		graphicsPipelineCreateInfo.SetStencilFunc(VK_COMPARE_OP_EQUAL, 1, 0xFF, 0);
		graphicsPipelineCreateInfo.SetStencilOp(VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP);
		// Default Multisampling already set in constructor
		graphicsPipelineCreateInfo.AddBlendAttachment(0xF, VK_TRUE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE,
			VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD);
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
		graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
		graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayoutLight, *m_pCompositeRenderPass);

		m_pPipelineLight = new VulkanPipeline(*m_pDevice);
		m_pPipelineLight->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		fs.Destroy();
	}
}

void DeferredRenderingRenderPass::CreateSyncResources()
{
	m_pDeferredCompleteSemaphore = new VulkanSemaphore(*m_pDevice);
	m_pDeferredCompleteSemaphore->Create();

	m_pDeferredFence = new VulkanFence(*m_pDevice);
	m_pDeferredFence->Create(VK_FENCE_CREATE_SIGNALED_BIT);
}


void DeferredRenderingRenderPass::RecordCommandBuffers()
{
	VkClearValue clearValues[3]{};
	clearValues[0].depthStencil = { 1.0f, 0 };
	clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };


	{
		m_DeferredCommandBuffer.BeginCommandBuffer();

		m_DeferredCommandBuffer.BeginRenderPass(*m_pDeferredRenderPass, *m_pDeferredFramebuffer, m_Width, m_Height, 3, clearValues);

		m_DeferredCommandBuffer.SetViewport(static_cast<float>(m_Width), static_cast<float>(m_Height));

		m_DeferredCommandBuffer.SetScissor(m_Width, m_Height);

		VkDeviceSize offsets[] = { 0 };
		VkBuffer vbs[] = { *m_pBoxVB };
		m_DeferredCommandBuffer.BindVertexBuffers(0, 1, vbs, offsets);

		m_DeferredCommandBuffer.BindIndexBuffer(*m_pBoxIB);

		m_DeferredCommandBuffer.BindPipeline(*m_pPipelineGBuffer);


		uint32_t offset;
		VkDescriptorSet sets[] = { *m_pDescriptorSetGBuffer };
		for (uint32_t j = 0; j < m_NumElements - 1; ++j)
		{
			offset = m_BufferAlignment * j;
			m_DeferredCommandBuffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutGBuffer, 0, 1, sets, 1, &offset);

			m_DeferredCommandBuffer.DrawIndexed(m_NumIndicesBox);
		}

		offset = m_BufferAlignment * (m_NumElements - 1);
		m_DeferredCommandBuffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutGBuffer, 0, 1, sets, 1, &offset);

		vbs[0] = *m_pGridVB;
		m_DeferredCommandBuffer.BindVertexBuffers(0, 1, vbs, offsets);
		m_DeferredCommandBuffer.BindIndexBuffer(*m_pGridIB);

		m_DeferredCommandBuffer.DrawIndexed(m_NumIndicesGrid);


		m_DeferredCommandBuffer.EndRenderPass();

		m_DeferredCommandBuffer.EndCommandBuffer();
	}

	{
		VkDescriptorSet sets[1]{};
		m_CommandBuffers[m_CurrImageIndex].BeginCommandBuffer();
		m_CommandBuffers[m_CurrImageIndex].BeginRenderPass(*m_pCompositeRenderPass, m_CompositeFramebuffers[m_CurrImageIndex], m_Width, m_Height, 2, clearValues);

		m_CommandBuffers[m_CurrImageIndex].SetViewport(static_cast<float>(m_Width), static_cast<float>(m_Height));

		m_CommandBuffers[m_CurrImageIndex].SetScissor(m_Width, m_Height);

		if (m_DebugTargets)
		{
			m_CommandBuffers[m_CurrImageIndex].BindPipeline(*m_pPipelineDebug);
			sets[0] = *m_pDescriptorSetDebug;
			m_CommandBuffers[m_CurrImageIndex].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutDebug, 0, 1, sets);
		}
		else
		{
			m_CommandBuffers[m_CurrImageIndex].BindPipeline(*m_pPipelineLight);
			sets[0] = *m_pDescriptorSetLight;
			m_CommandBuffers[m_CurrImageIndex].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutLight, 0, 1, sets);
		}
		m_CommandBuffers[m_CurrImageIndex].Draw(6);

		m_CommandBuffers[m_CurrImageIndex].EndRenderPass();
		m_CommandBuffers[m_CurrImageIndex].EndCommandBuffer();
	}
}


void DeferredRenderingRenderPass::Update()
{
	static float yVal = 3.0f;
	static float zVal = -16.0f;

	if (GetAsyncKeyState('U') & 0x8000) { yVal += 0.05f; }
	if (GetAsyncKeyState('J') & 0x8000) { yVal -= 0.05f; }
	if (GetAsyncKeyState('I') & 0x8000) { zVal += 0.05f; }
	if (GetAsyncKeyState('K') & 0x8000) { zVal -= 0.05f; }

	BvFastVec viewPos = VectorSet(0.0f, yVal, zVal);

	BvFastMat view = MatrixLookAtLH(viewPos, VectorSet(0.0f, yVal, zVal + 1.0f, 1.0f),
		VectorSet(0.0f, 1.0f, 0.0f));
	BvFastMat proj = MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4);
	BvFastMat viewProj = view * proj;
	Float44 projValues; Store44(proj, projValues.m);

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
	float z = 4.0f;
	for (uint32_t i = 0; i < m_NumElements - 1; ++i)
	{
		if (i % 5 == 0)
		{
			x = -4.0f;

			z -= 2.0f;
		}

		Store44(MatrixTranslation(x, 0.0f, z), pData.pAsUBData->w.m);
		Store44(Load44(pData.pAsUBData->w.m) * viewProj, pData.pAsUBData->wvp.m);

		pData.pAsChar += m_BufferAlignment;

		x += 2.0f;
	}

	Store44(MatrixTranslation(0.0f, -1.0f, 0.0f), pData.pAsUBData->w.m);
	Store44(Load44(pData.pAsUBData->w.m) * viewProj, pData.pAsUBData->wvp.m);

	static float r = 0.8f;
	static float g = 0.8f;
	static float b = 0.8f;

	if (GetAsyncKeyState('R') & 0x8000) { r += 0.01f; } if (r > 1.0f) { r = 1.0f; }
	if (GetAsyncKeyState('F') & 0x8000) { r -= 0.01f; } if (r < 0.0f) { r = 0.0f; }
	if (GetAsyncKeyState('T') & 0x8000) { g += 0.01f; } if (g > 1.0f) { g = 1.0f; }
	if (GetAsyncKeyState('G') & 0x8000) { g -= 0.01f; } if (g < 0.0f) { g = 0.0f; }
	if (GetAsyncKeyState('Y') & 0x8000) { b += 0.01f; } if (b > 1.0f) { b = 1.0f; }
	if (GetAsyncKeyState('H') & 0x8000) { b -= 0.01f; } if (b < 0.0f) { b = 0.0f; }

	UBOFrameData * pLightData = reinterpret_cast<UBOFrameData *>(m_pLightsUB->GetMappedData());
	Store3(viewPos, pLightData->viewPos.v);
	pLightData->light.m_LightType = LightType::Directional;
	Store3(VectorNormalize(VectorSet(-1.0f, -1.0f, 1.0f)), pLightData->light.m_Direction.v);
	pLightData->light.m_Color = Float3(r, g, b);

	GBufferUnpackData * pUnpackData = reinterpret_cast<GBufferUnpackData *>(m_pUnpackGBufferUB->GetMappedData());
	Store44(MatrixInverse(view), pUnpackData->invView.m);
	pUnpackData->projValues = Float4(1.0f / projValues.r[0].x, 1.0f / projValues.r[1].y, projValues.r[2].z, projValues.r[3].z);

	static float xCoord1 = 0.333f;
	static float xCoord2 = 0.666f;

	if (GetAsyncKeyState('1') & 0x8000) { m_DebugTargets = false; }
	if (GetAsyncKeyState('2') & 0x8000) { m_DebugTargets = true; }

	if (GetAsyncKeyState('C') & 0x8000) { xCoord1 -= 0.02f; }
	if (GetAsyncKeyState('V') & 0x8000) { xCoord1 += 0.02f; }
	if (GetAsyncKeyState('B') & 0x8000) { xCoord2 -= 0.02f; }
	if (GetAsyncKeyState('N') & 0x8000) { xCoord2 += 0.02f; }

	if (xCoord1 < 0.0f) { xCoord1 = 0.0f; }
	if (xCoord1 > xCoord2) { xCoord1 = xCoord2; }
	if (xCoord2 > 1.0f) { xCoord2 = 1.0f; }
	if (xCoord2 < xCoord1) { xCoord2 = xCoord1; }

	DebugData * pDebugData = reinterpret_cast<DebugData *>(m_pDebugUB->GetMappedData());
	pDebugData->debugValues = Float4(1.0f / static_cast<float>(m_Width), 1.0f / static_cast<float>(m_Height), xCoord1, xCoord2);
}


void DeferredRenderingRenderPass::Render()
{
	PrepareFrame();

	m_pDeferredFence->Wait();
	m_pDeferredFence->Reset();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();
	RecordCommandBuffers();

	m_pGraphicsQueue->Submit(m_DeferredCommandBuffer, *m_pPresentCompleteSemaphore, *m_pDeferredCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, *m_pDeferredFence);

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pDeferredCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}


CREATE_WIN_MAIN(DeferredRenderingRenderPass)