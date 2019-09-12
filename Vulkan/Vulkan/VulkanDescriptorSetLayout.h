#pragma once


#include "VulkanDevice.h"


struct DescriptorSetLayoutCreateInfo : VkDescriptorSetLayoutCreateInfo
{
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

	DescriptorSetLayoutCreateInfo()
		: VkDescriptorSetLayoutCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, 0, nullptr }
	{
	}

	inline void AddBinding(const uint32_t binding, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags,
		const uint32_t descriptorCount = 1, const VkSampler * const pImmutableSamplers = nullptr)
	{
		m_Bindings.push_back({ binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers });

		bindingCount = static_cast<uint32_t>(m_Bindings.size());
		pBindings = m_Bindings.data();
	}

	inline void ClearBindings()
	{
		m_Bindings.clear();

		bindingCount = 0;
		pBindings = nullptr;
	}
};


class VulkanDescriptorSetLayout
{
public:
	explicit VulkanDescriptorSetLayout(const VulkanDevice & device);
	~VulkanDescriptorSetLayout();

	VkResult Create(const VkDescriptorSetLayoutCreateInfo & createInfo);
	void Destroy();

	inline const VkDescriptorSetLayout Get() const { return m_SetLayout; }
	inline operator VkDescriptorSetLayout() const { return m_SetLayout; }

private:
	const VulkanDevice & m_Device;
	VkDescriptorSetLayout m_SetLayout = VK_NULL_HANDLE;
};