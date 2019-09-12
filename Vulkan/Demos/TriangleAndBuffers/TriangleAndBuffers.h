#pragma once


#include "VulkanBase.h"
#include "BvMath.h"

class ForwardRenderer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanPipelineCache;
class VulkanPipeline;
class VulkanPipelineLayout;
class VulkanBuffer;


class TriangleAndBuffers : public VulkanBase
{
public:
	TriangleAndBuffers();
	~TriangleAndBuffers();

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
	ForwardRenderer * m_pForwardRenderer;

	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;
	VulkanPipelineCache * m_pPipelineCache = nullptr;

	VulkanBuffer * m_pVB = nullptr;
	VulkanBuffer * m_pIB = nullptr;
	VulkanBuffer * m_pUB = nullptr;

	VulkanDescriptorPool * m_pDescriptorPool = nullptr;
	VulkanDescriptorSetLayout * m_pDescriptorSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;

	uint32_t m_NumIndices = 0;
};