#pragma once


#include "VulkanBase.h"
#include "VulkanPipelineCache.h"
#include "VulkanDescriptorPool.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "BvMath.h"


class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class VulkanPipeline;
class VulkanPipelineLayout;


class Skybox
{
public:
	Skybox();
	~Skybox();

	void Create(const VulkanDescriptorPool & descriptorPool, const VulkanPipelineCache & pipelineCache,
		const VulkanRenderPass & renderPass);
	void Destroy();

	void Set(const VulkanTexture & texture, const VulkanSampler & sampler);
	void Update(const Float3 & camPos, const Float44 & viewProj);
	void Render(const VulkanCommandBuffer & commandBuffer);

private:
	void CreateBuffers();
	void CreateUniformBuffers();

private:
	struct WVP
	{
		Float44 wvp;
	};

	VulkanBuffer * m_pVB = nullptr;
	VulkanBuffer * m_pIB = nullptr;
	VulkanBuffer * m_pUB = nullptr;

	VulkanDescriptorSetLayout * m_pSetLayout = nullptr;
	VulkanDescriptorSet * m_pDescriptorSet = nullptr;
	VulkanPipelineLayout * m_pPipelineLayout = nullptr;
	VulkanPipeline * m_pPipeline = nullptr;

	uint32_t m_IndexCount = 0;
};