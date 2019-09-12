#pragma once


#include "VulkanBase.h"
#include "BvMath.h"

class ForwardRenderer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanBuffer;
class VulkanTexture;
class VulkanSampler;
class VulkanPipelineCache;
class VulkanPipeline;
class VulkanPipelineLayout;


class TextureArray : public VulkanBase
{
public:
	TextureArray();
	~TextureArray();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreateBuffers();
	void CreateTextures();
	void CreateSamplers();
	void CreateUniformBuffers();
	void CreateDescriptorPools();
	void CreateDescriptorSetLayouts();
	void CreateDescriptorSets();
	void CreatePipelineLayouts();
	void CreatePipelineCaches();
	void CreateGraphicsPipelines();
	void RecordCommandBuffers();

	void Update() override;
	void Render() override;

private:
	struct UBData
	{
		Float44 m_WVP;
		uint32_t m_TextureIndex;
	};
	uint32_t m_BufferAlignment = 0;
	uint32_t m_NumElements = 6;

	ForwardRenderer * m_pForwardRenderer;

	VulkanPipelineCache * m_pPipelineCache = nullptr;
	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;

	VulkanBuffer * m_pVB = nullptr;
	VulkanBuffer * m_pIB = nullptr;
	VulkanBuffer * m_pDynamicUB = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;

	VulkanTexture * m_pTextureArray = nullptr;

	VulkanSampler * m_pSampler = nullptr;

	uint32_t m_NumIndices = 0;
};