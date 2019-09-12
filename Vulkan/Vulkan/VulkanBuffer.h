#pragma once


#include "VulkanDevice.h"


class VulkanBuffer
{
public:
	VulkanBuffer() = delete;
	explicit VulkanBuffer(const VulkanDevice & device);
	~VulkanBuffer();

	VkResult Create(const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size);
	VkResult Create(const VkBufferCreateInfo & buffercCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags);
	void Destroy();

	VkResult Map(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);
	void Unmap();

	VkResult Flush(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);
	VkResult Invalidate(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);

	void Update(const void * const pData, const VkDeviceSize size);

	inline void * const GetMappedData() const { return m_pMappedData; }
	inline const VkBuffer Get() const { return m_Buffer; }
	inline operator VkBuffer() const { return m_Buffer; }

private:
	const VulkanDevice & m_Device;
	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	void * m_pMappedData = nullptr;
};