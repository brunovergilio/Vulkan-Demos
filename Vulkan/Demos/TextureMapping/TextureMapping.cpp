#include "TextureMapping.h"
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


TextureMapping::TextureMapping()
{
	m_AppName = "Vulkan Demo - Texture Mapping";
}


TextureMapping::~TextureMapping()
{
}


void TextureMapping::Create(const VulkanRenderInfo & info)
{
	SetShaderDirectory("C:\\Programming\\C++\\Graphics\\Vulkan\\Assets - Shaders\\TextureMapping");

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
	RecordCommandBuffers();
}


void TextureMapping::Destroy()
{
	m_pPipelineCombined->Destroy(); BV_SAFE_DELETE(m_pPipelineCombined);
	m_pPipelineSeparate->Destroy(); BV_SAFE_DELETE(m_pPipelineSeparate);
	m_pPipelineCache->Destroy(); BV_SAFE_DELETE(m_pPipelineCache);
	m_pPipelineLayoutCombined->Destroy(); BV_SAFE_DELETE(m_pPipelineLayoutCombined);
	m_pPipelineLayoutSeparate->Destroy(); BV_SAFE_DELETE(m_pPipelineLayoutSeparate);

	m_pForwardRenderer->Destroy(); BV_SAFE_DELETE(m_pForwardRenderer);

	BV_SAFE_DELETE(m_pDescriptorSetCombined);
	BV_SAFE_DELETE(m_pDescriptorSetSeparate);
	m_pDescriptorSetLayoutCombined->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayoutCombined);
	m_pDescriptorSetLayoutSeparate->Destroy(); BV_SAFE_DELETE(m_pDescriptorSetLayoutSeparate);
	m_pDescriptorPool->Destroy(); BV_SAFE_DELETE(m_pDescriptorPool);

	m_pSampler->Destroy();
	m_pTextureCombined->Destroy();
	m_pTextureSeparate->Destroy();

	m_pUBCombined->Destroy(); BV_SAFE_DELETE(m_pUBCombined);
	m_pUBSeparate->Destroy(); BV_SAFE_DELETE(m_pUBSeparate);
	m_pVB->Destroy(); BV_SAFE_DELETE(m_pVB);
	m_pIB->Destroy(); BV_SAFE_DELETE(m_pIB);

	VulkanBase::Destroy();
}


void TextureMapping::Resize()
{
	VulkanBase::Resize();

	m_pForwardRenderer->Resize();

	RecordCommandBuffers();
}


void TextureMapping::CreateRenderers()
{
	m_pForwardRenderer = new ForwardRenderer();
	m_pForwardRenderer->Create();
}


void TextureMapping::CreateBuffers()
{
	BvVertexList vertices;
	BvIndexList<uint32_t> indices;

	BvVertexLayout layouts[] =
	{
		BV_VERTEX_POSITION,
		BV_VERTEX_TEXCOORD
	};

	CreateBox(BV_VERTEX_POSITION | BV_VERTEX_TEXCOORD, vertices, indices);
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


void TextureMapping::CreateTextures()
{
	std::string texName = GetTextureFilePath("metal.dds");

	m_pTextureCombined = new VulkanTexture();
	m_pTextureCombined->CreateFromFile(texName.c_str());

	texName = GetTextureFilePath("white_marble.dds");

	m_pTextureSeparate = new VulkanTexture();
	m_pTextureSeparate->CreateFromFile(texName.c_str());
}


void TextureMapping::CreateSamplers()
{
	SamplerCreateInfo samplerCreateInfo;
	// Fine to set to one of the texture's level count since both textures
	// have the same number of mip levels
	samplerCreateInfo.SetAnisotropy(VK_TRUE, static_cast<float>(m_pTextureCombined->GetLevelCount()));
	m_pSampler = new VulkanSampler(*m_pDevice);
	m_pSampler->Create(samplerCreateInfo);
}


void TextureMapping::CreateUniformBuffers()
{
	m_pUBCombined = new VulkanBuffer(*m_pDevice);
	m_pUBCombined->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float44));
	m_pUBCombined->Map();

	m_pUBSeparate = new VulkanBuffer(*m_pDevice);
	m_pUBSeparate->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(Float44));
	m_pUBSeparate->Map();
}


void TextureMapping::CreateDescriptorPools()
{
	DescriptorPoolCreateInfo poolInfo(2);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1);
	poolInfo.AddPoolType(VK_DESCRIPTOR_TYPE_SAMPLER, 1);
	m_pDescriptorPool = new VulkanDescriptorPool(*m_pDevice);
	m_pDescriptorPool->Create(poolInfo);
}


void TextureMapping::CreateDescriptorSetLayouts()
{
	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayoutCombined = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayoutCombined->Create(setLayoutInfo);
	}

	{
		DescriptorSetLayoutCreateInfo setLayoutInfo;
		setLayoutInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		setLayoutInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT);
		setLayoutInfo.AddBinding(2, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_pDescriptorSetLayoutSeparate = new VulkanDescriptorSetLayout(*m_pDevice);
		m_pDescriptorSetLayoutSeparate->Create(setLayoutInfo);
	}
}


void TextureMapping::CreateDescriptorSets()
{
	{
		m_pDescriptorSetCombined = new VulkanDescriptorSet(*m_pDevice);
		m_pDescriptorPool->Allocate(m_pDescriptorSetCombined, m_pDescriptorSetLayoutCombined);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = *m_pUBCombined;
		bufferInfo.range = sizeof(Float44);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *m_pTextureCombined->GetView();
		imageInfo.sampler = *m_pSampler;

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
		};
		m_pDescriptorSetCombined->Update(2, writeSets);
	}

	{
		m_pDescriptorSetSeparate = new VulkanDescriptorSet(*m_pDevice);
		m_pDescriptorPool->Allocate(m_pDescriptorSetSeparate, m_pDescriptorSetLayoutSeparate);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = *m_pUBSeparate;
		bufferInfo.range = sizeof(Float44);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *m_pTextureSeparate->GetView();
		imageInfo.sampler = VK_NULL_HANDLE;

		VkDescriptorImageInfo samplerInfo{};
		samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		samplerInfo.imageView = VK_NULL_HANDLE;
		samplerInfo.sampler = *m_pSampler;

		WriteDescriptorSet writeSets[] =
		{
			WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo),
			WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, &imageInfo),
			WriteDescriptorSet(2, VK_DESCRIPTOR_TYPE_SAMPLER, &samplerInfo)
		};
		m_pDescriptorSetSeparate->Update(3, writeSets);
	}
}


void TextureMapping::CreatePipelineLayouts()
{
	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayoutCombined);
		m_pPipelineLayoutCombined = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayoutCombined->Create(pipelineLayoutCreateInfo);
	}

	{
		PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.AddDescriptorSetLayout(*m_pDescriptorSetLayoutSeparate);
		m_pPipelineLayoutSeparate = new VulkanPipelineLayout(*m_pDevice);
		m_pPipelineLayoutSeparate->Create(pipelineLayoutCreateInfo);
	}
}


void TextureMapping::CreatePipelineCaches()
{
	m_pPipelineCache = new VulkanPipelineCache(*m_pDevice);
	m_pPipelineCache->Create();
}


void TextureMapping::CreateGraphicsPipelines()
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

	{
		auto vsShader = GetShaderFilePath("vsCombined.spv");
		auto fsShader = GetShaderFilePath("fsCombined.spv");

		VulkanShader vs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		fs.Create(fsShader.c_str());

		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
		graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayoutCombined, *m_pForwardRenderer->GetRenderPass(), 0);

		m_pPipelineCombined = new VulkanPipeline(*m_pDevice);
		m_pPipelineCombined->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		fs.Destroy();
	}

	graphicsPipelineCreateInfo.ClearShaderStages();

	{
		auto vsShader = GetShaderFilePath("vsSeparate.spv");
		auto fsShader = GetShaderFilePath("fsSeparate.spv");

		VulkanShader vs(*m_pDevice), fs(*m_pDevice);
		vs.Create(vsShader.c_str());
		fs.Create(fsShader.c_str());

		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs);
		graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs);
		graphicsPipelineCreateInfo.SetRenderInfo(*m_pPipelineLayoutSeparate, *m_pForwardRenderer->GetRenderPass(), 0);

		m_pPipelineSeparate = new VulkanPipeline(*m_pDevice);
		m_pPipelineSeparate->Create(graphicsPipelineCreateInfo, *m_pPipelineCache);

		vs.Destroy();
		fs.Destroy();
	}
}


void TextureMapping::RecordCommandBuffers()
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

		m_CommandBuffers[i].BindPipeline(*m_pPipelineCombined);

		{
			VkDescriptorSet sets[] = { *m_pDescriptorSetCombined };
			m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutCombined, 0, 1, sets);
		}

		m_CommandBuffers[i].DrawIndexed(m_NumIndices);

		m_CommandBuffers[i].BindPipeline(*m_pPipelineSeparate);

		{
			VkDescriptorSet sets[] = { *m_pDescriptorSetSeparate };
			m_CommandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pPipelineLayoutSeparate, 0, 1, sets);
		}

		m_CommandBuffers[i].DrawIndexed(m_NumIndices);

		m_CommandBuffers[i].EndRenderPass();

		m_CommandBuffers[i].EndCommandBuffer();
	}
}


void TextureMapping::Update()
{
	static float rotX = 0.0f;
	static float rotY = 0.0f;

	if (rotX > Bv2Pi) { rotX = 0.0f; }
	if (rotY > Bv2Pi) { rotY = 0.0f; }

	Float44 * wvpCombined = reinterpret_cast<Float44 *>(m_pUBCombined->GetMappedData());
	Store44(MatrixRotationY(rotX)
		* MatrixRotationX(rotY)
		* MatrixTranslation(-2.0f, 0.0f, 0.0f)
		* MatrixLookAtLH(VectorSet(0.0f, 0.0f, -6.0f, 1.0f), VectorSet(0.0f, 0.0f, 1.0f, 1.0f),
			VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4), wvpCombined->m);

	Float44 * wvpSeparate = reinterpret_cast<Float44 *>(m_pUBSeparate->GetMappedData());
	Store44(MatrixRotationY(rotX)
		* MatrixRotationX(rotY)
		* MatrixTranslation(+2.0f, 0.0f, 0.0f)
		* MatrixLookAtLH(VectorSet(0.0f, 0.0f, -6.0f, 1.0f), VectorSet(0.0f, 0.0f, 1.0f, 1.0f),
			VectorSet(0.0f, 1.0f, 0.0f))
		* MatrixPerspectiveLH_VK(0.1f, 100.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), BvPiDiv4), wvpSeparate->m);

	rotX += 0.002f;
	rotY += 0.003f;
}


void TextureMapping::Render()
{
	PrepareFrame();

	m_Fences[m_CurrImageIndex].Wait();
	m_Fences[m_CurrImageIndex].Reset();

	m_pGraphicsQueue->Submit(m_CommandBuffers[m_CurrImageIndex], *m_pPresentCompleteSemaphore, *m_pRenderCompleteSemaphore,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_Fences[m_CurrImageIndex]);

	PresentFrame();
}