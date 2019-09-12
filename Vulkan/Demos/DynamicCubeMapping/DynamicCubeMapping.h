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
class Skybox;
class VulkanPipelineCache;
class VulkanPipeline;
class VulkanPipelineLayout;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanFence;
class VulkanSemaphore;


class DynamicCubeMapping : public VulkanBase
{
public:
	DynamicCubeMapping();
	~DynamicCubeMapping();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreateBuffers();
	void CreateDCMFramebuffers();
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
	void CreateSyncResources();
	void RecordDCMCommandBuffers();
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

	VulkanBuffer * m_pSphereVB = nullptr;
	VulkanBuffer * m_pSphereIB = nullptr;
	VulkanBuffer * m_pCubeVB = nullptr;
	VulkanBuffer * m_pCubeIB = nullptr;
	uint32_t m_SphereIndices = 0;
	uint32_t m_CubeIndices = 0;

	VulkanBuffer * m_pUBWVP = nullptr;
	VulkanBuffer * m_pUBCam = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;

	VulkanTexture * m_pSkyTexture = nullptr;
	VulkanTexture * m_pMarbleTexture = nullptr;
	VulkanSampler * m_pSkySampler = nullptr;
	VulkanSampler * m_pMarbleSampler = nullptr;

	Skybox * m_pSkybox = nullptr;

	// Dynamic Cube Map stuff
	VulkanTexture * m_pColorTextureCube = nullptr;
	VulkanTexture * m_pDepthTextureCube = nullptr;

	VulkanRenderPass * m_pDCMRenderPass = nullptr;
	VulkanFramebuffer * m_pDCMFramebuffer = nullptr;

	VulkanDescriptorSetLayout * m_pDCMDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDCMDescriptorSet = nullptr;
	VulkanPipelineLayout * m_pDCMPipelineLayout = nullptr;
	VulkanPipeline * m_pDCMPipeline = nullptr;
	VulkanBuffer * m_pDCMUB = nullptr;

	VulkanCommandBuffer * m_pDCMCommandBuffer = nullptr;

	VulkanSemaphore * m_pDCMSemaphore = nullptr;
	VulkanFence * m_pDCMFence = nullptr;
};