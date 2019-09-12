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


class SimpleFog : public VulkanBase
{
public:
	SimpleFog();
	~SimpleFog();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreateBuffers();
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
	struct UBOFrameData
	{
		Light light;
		Float3 viewPos;
		uint32_t fogType;
		Float3 insColor;
		float insDensity;
	};

	uint32_t m_BufferAlignment = 0;
	uint32_t m_NumElements = 26;

	ForwardRenderer * m_pForwardRenderer;

	VulkanPipelineCache * m_pPipelineCache = nullptr;
	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;

	VulkanBuffer * m_pGridVB = nullptr;
	VulkanBuffer * m_pGridIB = nullptr;

	VulkanBuffer * m_pBoxVB = nullptr;
	VulkanBuffer * m_pBoxIB = nullptr;

	VulkanBuffer * m_pDynamicUB = nullptr;
	VulkanBuffer * m_pLightsUB = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;

	uint32_t m_NumIndicesBox = 0;
	uint32_t m_NumIndicesGrid = 0;
};