#pragma once


#include "VulkanDevice.h"


struct PipelineLayoutCreateInfo : VkPipelineLayoutCreateInfo
{
	std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
	std::vector<VkPushConstantRange> m_PushConstants;

	PipelineLayoutCreateInfo()
		: VkPipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr }
	{
	}

	inline void AddDescriptorSetLayout(const VkDescriptorSetLayout descriptorSetLayout)
	{
		m_DescriptorSetLayouts.push_back(descriptorSetLayout);

		setLayoutCount = static_cast<uint32_t>(m_DescriptorSetLayouts.size());
		pSetLayouts = m_DescriptorSetLayouts.data();
	}

	inline void ClearDescriptorSetLayouts()
	{
		m_DescriptorSetLayouts.clear();

		setLayoutCount = 0;
		pSetLayouts = nullptr;
	}

	inline void AddPushConstant(const VkShaderStageFlags stageFlags, const uint32_t size, const uint32_t offset)
	{
		m_PushConstants.push_back({ stageFlags, offset, size });

		pushConstantRangeCount = static_cast<uint32_t>(m_PushConstants.size());
		pPushConstantRanges = m_PushConstants.data();
	}

	inline void ClearPushConstants()
	{
		m_PushConstants.clear();

		pushConstantRangeCount = 0;
		pPushConstantRanges = nullptr;
	}
};


class VulkanPipelineLayout
{
public:
	explicit VulkanPipelineLayout(const VulkanDevice & device);
	~VulkanPipelineLayout();

	VkResult Create(const VkPipelineLayoutCreateInfo & createInfo);
	void Destroy();

	inline const VkPipelineLayout Get() const { return m_Layout; }
	inline operator VkPipelineLayout() const { return m_Layout; }

private:
	const VulkanDevice & m_Device;
	VkPipelineLayout m_Layout = VK_NULL_HANDLE;
};