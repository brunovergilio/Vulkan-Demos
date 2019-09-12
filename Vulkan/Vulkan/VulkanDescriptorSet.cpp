#include "VulkanDescriptorSet.h"


VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanDescriptorSet::~VulkanDescriptorSet()
{
}


void VulkanDescriptorSet::Update(const uint32_t writeCount, VkWriteDescriptorSet * const pWriteSets)
{
	for (auto i = 0U; i < writeCount; ++i)
	{
		pWriteSets[i].dstSet = m_Set;
	}

	vkUpdateDescriptorSets(m_Device, writeCount, pWriteSets, 0, nullptr);
}