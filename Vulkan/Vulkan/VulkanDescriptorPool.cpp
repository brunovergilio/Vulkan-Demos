#include "VulkanDescriptorPool.h"


VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanDescriptorPool::~VulkanDescriptorPool()
{
	Destroy();
}


VkResult VulkanDescriptorPool::Create(const VkDescriptorPoolCreateInfo & createInfo)
{
	return vkCreateDescriptorPool(m_Device, &createInfo, nullptr, &m_Pool);
}


void VulkanDescriptorPool::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Pool, nullptr, DescriptorPool);
}


VkResult VulkanDescriptorPool::Allocate(VulkanDescriptorSet * const pDescriptorSet, const VulkanDescriptorSetLayout * const pSetLayout) const
{
	assert(m_Pool != VK_NULL_HANDLE);
	assert(pDescriptorSet != nullptr);

	VkDescriptorSetLayout setLayout = pSetLayout->Get();

	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = m_Pool;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &setLayout;

	return vkAllocateDescriptorSets(m_Device, &allocateInfo, &pDescriptorSet->m_Set);
}


void VulkanDescriptorPool::Free(VulkanDescriptorSet * const pDescriptorSet) const
{
	assert(m_Pool != VK_NULL_HANDLE);
	assert(pDescriptorSet != nullptr);

	VK_SAFE_FREE(m_Device, m_Pool, 1, &pDescriptorSet->m_Set, DescriptorSet);
}


VkResult VulkanDescriptorPool::AllocateMultiple(const uint32_t count, VulkanDescriptorSet * const pDescriptorSets, const VulkanDescriptorSetLayout * const pSetLayout) const
{
	assert(m_Pool != VK_NULL_HANDLE);
	assert(pDescriptorSets != nullptr);
	assert(count > 0);

	VkDescriptorSetLayout setLayout = pSetLayout->Get();

	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = m_Pool;
	allocateInfo.descriptorSetCount = count;
	allocateInfo.pSetLayouts = &setLayout;

	return vkAllocateDescriptorSets(m_Device, &allocateInfo, reinterpret_cast<VkDescriptorSet *>(pDescriptorSets));
}


void VulkanDescriptorPool::FreeMultiple(const uint32_t count, VulkanDescriptorSet * const pDescriptorSets) const
{
	assert(m_Pool != VK_NULL_HANDLE);
	assert(pDescriptorSets != nullptr);
	assert(count > 0);

	VK_SAFE_FREE(m_Device, m_Pool, count, reinterpret_cast<VkDescriptorSet *>(pDescriptorSets), DescriptorSet);
}