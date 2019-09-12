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
class Skybox;


class CubeMapping : public VulkanBase
{
public:
	CubeMapping();
	~CubeMapping();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreateBuffers();
	void CreateTextures();
	void CreateSamplers();
	void CreateSkybox();
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
	struct WVP
	{
		Float44 w;
		Float44 vp;
	};

	ForwardRenderer * m_pForwardRenderer;

	VulkanPipelineCache * m_pPipelineCache = nullptr;
	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;

	VulkanBuffer * m_pVB = nullptr;
	VulkanBuffer * m_pIB = nullptr;
	VulkanBuffer * m_pUBWVP = nullptr;
	VulkanBuffer * m_pUBCam = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;

	VulkanTexture * m_pSkyTexture = nullptr;

	VulkanSampler * m_pSkySampler = nullptr;

	Skybox * m_pSkybox = nullptr;

	uint32_t m_NumIndices = 0;
};