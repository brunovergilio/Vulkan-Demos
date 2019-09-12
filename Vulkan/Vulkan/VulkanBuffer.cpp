#include "VulkanBuffer.h"


VulkanBuffer::VulkanBuffer(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanBuffer::~VulkanBuffer()
{
	Destroy();
}


VkResult VulkanBuffer::Create(const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size)
{
	VkBufferCreateInfo buffercCreateInfo{};
	buffercCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffercCreateInfo.usage = usageFlags;
	buffercCreateInfo.size = size;

	return Create(buffercCreateInfo, memoryPropertyFlags);
}


VkResult VulkanBuffer::Create(const VkBufferCreateInfo & buffercCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
{
	VkResult result;
	if ((result = vkCreateBuffer(m_Device, &buffercCreateInfo, nullptr, &m_Buffer)) != VK_SUCCESS)
	{
		return result;
	}

	VkMemoryRequirements reqs{};
	vkGetBufferMemoryRequirements(m_Device, m_Buffer, &reqs);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = reqs.size;
	allocateInfo.memoryTypeIndex = m_Device.GetMemoryTypeIndex(reqs.memoryTypeBits, memoryPropertyFlags);

	if ((result = vkAllocateMemory(m_Device, &allocateInfo, nullptr, &m_Memory)) != VK_SUCCESS)
	{
		VK_SAFE_DESTROY(m_Device, m_Buffer, nullptr, Buffer);

		return result;
	}

	if ((result = vkBindBufferMemory(m_Device, m_Buffer, m_Memory, 0)) != VK_SUCCESS)
	{
		VK_SAFE_FREE_MEMORY(m_Device, m_Memory, nullptr);
		VK_SAFE_DESTROY(m_Device, m_Buffer, nullptr, Buffer);

		return result;
	}

	return VK_SUCCESS;
}


void VulkanBuffer::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Buffer, nullptr, Buffer);
	VK_SAFE_FREE_MEMORY(m_Device, m_Memory, nullptr);
}


VkResult VulkanBuffer::Map(const VkDeviceSize size, const VkDeviceSize offset)
{
	assert(nullptr == m_pMappedData);

	return vkMapMemory(m_Device, m_Memory, offset, size, 0, &m_pMappedData);
}


void VulkanBuffer::Unmap()
{
	assert(m_pMappedData != nullptr);

	vkUnmapMemory(m_Device, m_Memory);

	m_pMappedData = nullptr;
}


VkResult VulkanBuffer::Flush(const VkDeviceSize size, const VkDeviceSize offset)
{
	VkMappedMemoryRange mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.offset = offset;
	mappedMemoryRange.size = size;
	mappedMemoryRange.memory = m_Memory;

	return vkFlushMappedMemoryRanges(m_Device, 1, &mappedMemoryRange);
}


VkResult VulkanBuffer::Invalidate(const VkDeviceSize size, const VkDeviceSize offset)
{
	VkMappedMemoryRange mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.offset = offset;
	mappedMemoryRange.size = size;
	mappedMemoryRange.memory = m_Memory;

	return vkInvalidateMappedMemoryRanges(m_Device, 1, &mappedMemoryRange);
}


void VulkanBuffer::Update(const void * const pData, const VkDeviceSize size)
{
	assert(m_pMappedData != nullptr);

	memcpy(m_pMappedData, pData, static_cast<size_t>(size));
}