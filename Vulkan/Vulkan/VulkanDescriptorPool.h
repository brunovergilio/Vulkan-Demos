#pragma once


#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"


#define MAX_DESCRIPTOR_SET_ALLOCATIONS 64


struct DescriptorPoolCreateInfo : VkDescriptorPoolCreateInfo
{
	std::vector<VkDescriptorPoolSize> m_PoolSizes;
	
	DescriptorPoolCreateInfo()
		: VkDescriptorPoolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0, 0, 0, nullptr }
	{
	}

	DescriptorPoolCreateInfo(const uint32_t maxSets)
		: VkDescriptorPoolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0, maxSets, 0, nullptr }
	{
	}

	void AddPoolType(const VkDescriptorType type, const uint32_t descriptorCount)
	{
		m_PoolSizes.push_back(VkDescriptorPoolSize{ type, descriptorCount });
		
		poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
		pPoolSizes = m_PoolSizes.data();
	}

	void ClearPoolTypes()
	{
		m_PoolSizes.clear();

		poolSizeCount = 0;
		pPoolSizes = nullptr;
	}

	void SetMaxSets(const uint32_t maxSets)
	{
		this->maxSets = maxSets;
	}
};


class VulkanDescriptorPool
{
public:
	explicit VulkanDescriptorPool(const VulkanDevice & device);
	~VulkanDescriptorPool();

	VkResult Create(const VkDescriptorPoolCreateInfo & createInfo);
	void Destroy();

	VkResult Allocate(VulkanDescriptorSet * const pDescriptorSet, const VulkanDescriptorSetLayout * const pSetLayout) const;
	void Free(VulkanDescriptorSet * const pDescriptorSet) const;

	VkResult AllocateMultiple(const uint32_t count, VulkanDescriptorSet * const pDescriptorSets, const VulkanDescriptorSetLayout * const pSetLayout) const;
	void FreeMultiple(const uint32_t count, VulkanDescriptorSet * const pDescriptorSets) const;

	inline const VkDescriptorPool Get() const { return m_Pool; }
	inline operator VkDescriptorPool() const { return m_Pool; }

private:
	const VulkanDevice & m_Device;
	VkDescriptorPool m_Pool = VK_NULL_HANDLE;
};