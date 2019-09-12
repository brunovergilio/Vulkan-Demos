#pragma once


#include "VulkanDevice.h"


struct DescriptorBufferInfo : public VkDescriptorBufferInfo
{
	DescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize range = VK_WHOLE_SIZE, const VkDeviceSize offset = 0)
		: VkDescriptorBufferInfo{ buffer, offset, range }
	{
	}
};


struct DescriptorImageInfo : public VkDescriptorImageInfo
{
	DescriptorImageInfo(const VkSampler sampler, const VkImageView imageView, const VkImageLayout imageLayout)
		: VkDescriptorImageInfo{ sampler, imageView, imageLayout }
	{
	}
};


struct WriteDescriptorSet : public VkWriteDescriptorSet
{
	WriteDescriptorSet()
		: VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, 0, 0, 0, 0, VK_DESCRIPTOR_TYPE_MAX_ENUM,
			nullptr, nullptr, nullptr}
	{
	}

	WriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descriptorType, const VkDescriptorBufferInfo * const pBufferInfo,
		const uint32_t descriptorCount = 1, const uint32_t dstArrayElement = 0)
		: VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, 0, dstBinding, dstArrayElement,
			descriptorCount, descriptorType,
			nullptr, pBufferInfo, nullptr }
	{
	}

	WriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descriptorType, const VkDescriptorImageInfo * const pImageInfo,
		const uint32_t descriptorCount = 1, const uint32_t dstArrayElement = 0)
		: VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, 0, dstBinding, dstArrayElement,
			descriptorCount, descriptorType,
			pImageInfo, nullptr, nullptr }
	{
	}

	WriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descriptorType, const VkBufferView * const pTexelBufferView,
		const uint32_t descriptorCount = 1, const uint32_t dstArrayElement = 0)
		: VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, 0, dstBinding, dstArrayElement,
			descriptorCount, descriptorType,
			nullptr, nullptr, pTexelBufferView }
	{
	}
};


class VulkanDescriptorSet
{
public:
	friend class VulkanDescriptorPool;

	VulkanDescriptorSet(const VulkanDevice & device);
	~VulkanDescriptorSet();

	void Update(const uint32_t writeCount, VkWriteDescriptorSet * const pWriteSets);

	inline const VkDescriptorSet Get() const { return m_Set; }
	inline operator VkDescriptorSet() const { return m_Set; }

private:
	const VulkanDevice & m_Device;
	VkDescriptorSet m_Set = VK_NULL_HANDLE;
};