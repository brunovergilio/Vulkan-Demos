#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"


VulkanCommandPool::VulkanCommandPool(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanCommandPool::~VulkanCommandPool()
{
	Destroy();
}


VkResult VulkanCommandPool::Create(const uint32_t & queueIndex, const VkFlags flags)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = queueIndex;
	commandPoolCreateInfo.flags = flags;

	return vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPool);
}


void VulkanCommandPool::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_CommandPool, nullptr, CommandPool);
}


VkResult VulkanCommandPool::Allocate(VulkanCommandBuffer * const pCommandBuffer, const VkCommandBufferLevel level) const
{
	assert(m_CommandPool != VK_NULL_HANDLE);
	assert(pCommandBuffer != nullptr);

	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = m_CommandPool;
	allocateInfo.level = level;
	allocateInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(m_Device, &allocateInfo, &pCommandBuffer->m_CommandBuffer);
}


void VulkanCommandPool::Free(VulkanCommandBuffer * const pCommandBuffer) const
{
	assert(m_CommandPool != VK_NULL_HANDLE);
	assert(pCommandBuffer != nullptr);

	VK_SAFE_FREE(m_Device, m_CommandPool, 1, &pCommandBuffer->m_CommandBuffer, CommandBuffer);
}


VkResult VulkanCommandPool::AllocateMultiple(const uint32_t count, VulkanCommandBuffer * const pCommandBuffers,
	const VkCommandBufferLevel level) const
{
	assert(m_CommandPool != VK_NULL_HANDLE);
	assert(pCommandBuffers != nullptr);
	assert(count > 0);

	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = m_CommandPool;
	allocateInfo.level = level;
	allocateInfo.commandBufferCount = count;

	std::vector<VkCommandBuffer> commandBuffers(count);

	VkResult result = vkAllocateCommandBuffers(m_Device, &allocateInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		return result;
	}

	for (auto i = 0U; i < count; ++i)
	{
		pCommandBuffers[i].m_CommandBuffer = commandBuffers[i];
	}

	return VK_SUCCESS;
}


void VulkanCommandPool::FreeMultiple(const uint32_t count, VulkanCommandBuffer * const pCommandBuffers) const
{
	assert(m_CommandPool != VK_NULL_HANDLE);
	assert(pCommandBuffers != nullptr);
	assert(count > 0);

	std::vector<VkCommandBuffer> commandBuffers(count);
	for (auto i = 0U; i < count; ++i)
	{
		commandBuffers[i] = pCommandBuffers[i];
	}

	VK_SAFE_FREE(m_Device, m_CommandPool, count, commandBuffers.data(), CommandBuffer);
}


VkResult VulkanCommandPool::Reset(const VkFlags flags) const
{
	return vkResetCommandPool(m_Device, m_CommandPool, flags);
}