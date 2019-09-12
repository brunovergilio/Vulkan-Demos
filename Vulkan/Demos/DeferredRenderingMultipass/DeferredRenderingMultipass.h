#pragma once


#include "VulkanBase.h"
#include "BvMath.h"
#include "Lights.h"

class ForwardRenderer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanBuffer;
class VulkanPipelineCache;
class VulkanPipeline;
class VulkanPipelineLayout;
class VulkanTexture;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanSampler;
class VulkanImageView;


class DeferredRenderingMultipass : public VulkanBase
{
public:
	DeferredRenderingMultipass();
	~DeferredRenderingMultipass();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreateBuffers();
	void CreateUniformBuffers();
	void CreateTextures();
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
	struct UBOFrameData
	{
		Light light;
		Float3 viewPos;
	};

	struct GBufferUnpackData
	{
		Float44 invView;
		Float4 projValues;
	};

	struct DebugData
	{
		Float4 debugValues;
	};

	uint32_t m_BufferAlignment = 0;
	uint32_t m_NumElements = 26;

	VulkanTexture * m_pDepthTex = nullptr;
	VulkanTexture * m_pDiffuseTex = nullptr;
	VulkanTexture * m_pNormalTex = nullptr;
	VulkanImageView * m_pDepthReadOnlyView = nullptr;
	VulkanRenderPass * m_pDeferredRenderPass = nullptr;
	std::vector<VulkanFramebuffer> m_DeferredFramebuffers;

	VulkanTexture * m_pTexture = nullptr;
	VulkanSampler * m_pSampler = nullptr;

	VulkanPipelineCache * m_pPipelineCache = nullptr;
	VulkanPipelineLayout * m_pPipelineLayoutGBuffer = nullptr;
	VulkanPipelineLayout * m_pPipelineLayoutDebug = nullptr;
	VulkanPipelineLayout * m_pPipelineLayoutLight = nullptr;
	VulkanPipeline * m_pPipelineGBuffer = nullptr;
	VulkanPipeline * m_pPipelineDebug = nullptr;
	VulkanPipeline * m_pPipelineLight = nullptr;

	VulkanBuffer * m_pGridVB = nullptr;
	VulkanBuffer * m_pGridIB = nullptr;

	VulkanBuffer * m_pBoxVB = nullptr;
	VulkanBuffer * m_pBoxIB = nullptr;

	VulkanBuffer * m_pDynamicUB = nullptr;
	VulkanBuffer * m_pLightsUB = nullptr;
	VulkanBuffer * m_pDebugUB = nullptr;
	VulkanBuffer * m_pUnpackGBufferUB = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayoutGBuffer = nullptr;
	VulkanDescriptorSet * m_pDescriptorSetGBuffer = nullptr;

	VulkanDescriptorSetLayout * m_pDescriptorSetLayoutDebug = nullptr;
	VulkanDescriptorSet * m_pDescriptorSetDebug = nullptr;
	
	VulkanDescriptorSetLayout * m_pDescriptorSetLayoutLight = nullptr;
	VulkanDescriptorSet * m_pDescriptorSetLight = nullptr;

	uint32_t m_NumIndicesBox = 0;
	uint32_t m_NumIndicesGrid = 0;

	bool m_DebugTargets = false;
};