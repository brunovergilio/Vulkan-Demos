#pragma once


#include "VulkanDevice.h"


class VulkanCommandBuffer;


class VulkanCommandPool
{
public:
	explicit VulkanCommandPool(const VulkanDevice & device);
	~VulkanCommandPool();

	VkResult Create(const uint32_t & queueIndex, const VkFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	void Destroy();

	VkResult Allocate(VulkanCommandBuffer * const pCommandBuffer, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
	void Free(VulkanCommandBuffer * const pCommandBuffer) const;

	VkResult AllocateMultiple(const uint32_t count, VulkanCommandBuffer * const pCommandBuffers,
		const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
	void FreeMultiple(const uint32_t count, VulkanCommandBuffer * const pCommandBuffers) const;

	VkResult Reset(const VkFlags flags = 0) const;

	inline const VkCommandPool Get() const { return m_CommandPool; }
	inline operator VkCommandPool() const { return m_CommandPool; }

private:
	const VulkanDevice & m_Device;
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
};