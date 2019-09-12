#pragma once


#include "VulkanBase.h"


class ForwardRenderer;
class VulkanPipelineCache;
class VulkanPipelineLayout;
class VulkanPipeline;


class SimpleTriangle : public VulkanBase
{
public:
	SimpleTriangle();
	~SimpleTriangle();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void CreatePipelineLayouts();
	void CreatePipelineCache();
	void CreateGraphicsPipelines();
	void RecordCommandBuffers();

	void Update() override {};
	void Render() override;

private:
	ForwardRenderer * m_pForwardRenderer = nullptr;

	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;
	VulkanPipelineCache * m_pPipelineCache = nullptr;
};