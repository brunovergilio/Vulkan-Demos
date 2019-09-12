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


class TextureMapping : public VulkanBase
{
public:
	TextureMapping();
	~TextureMapping();

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
	ForwardRenderer * m_pForwardRenderer;

	VulkanPipelineCache * m_pPipelineCache = nullptr;

	VulkanPipelineLayout * m_pPipelineLayoutCombined = nullptr;
	VulkanPipeline * m_pPipelineCombined = nullptr;

	VulkanPipelineLayout * m_pPipelineLayoutSeparate = nullptr;
	VulkanPipeline * m_pPipelineSeparate = nullptr;

	VulkanBuffer * m_pVB = nullptr;
	VulkanBuffer * m_pIB = nullptr;
	VulkanBuffer * m_pUBCombined = nullptr;
	VulkanBuffer * m_pUBSeparate = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;

	VulkanDescriptorSetLayout * m_pDescriptorSetLayoutCombined = nullptr;
	VulkanDescriptorSet * m_pDescriptorSetCombined = nullptr;

	VulkanDescriptorSetLayout * m_pDescriptorSetLayoutSeparate = nullptr;
	VulkanDescriptorSet * m_pDescriptorSetSeparate = nullptr;

	VulkanTexture * m_pTextureCombined = nullptr;
	VulkanTexture * m_pTextureSeparate = nullptr;

	VulkanSampler * m_pSampler = nullptr;

	uint32_t m_NumIndices = 0;
};