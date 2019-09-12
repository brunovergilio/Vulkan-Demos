#pragma once


#include "VulkanInstance.h"


class VulkanSurface;
class VulkanQueue;


class VulkanDevice
{
public:
	VulkanDevice(const VulkanGPUInfo & gpuInfo);
	~VulkanDevice();

	VkResult Create(VulkanSurface * pSurface = nullptr,
		const VkFlags queues = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT,
		const bool debugSupport = true);
	void Destroy();

	VkResult WaitIdle();

	const uint32_t GetQueueFamilyIndex(const VkQueueFlags queueFlags) const;
	const uint32_t GetMemoryTypeIndex(const uint32_t memoryTypeBits, const VkMemoryPropertyFlags properties) const;
	const VkFormat GetBestDepthFormat(const VkFormat format = VK_FORMAT_UNDEFINED) const;

	inline const VulkanGPUInfo & GetGPUInfo() const { return m_GPUInfo; }

	inline VulkanQueue * const GetGraphicsQueue() const { return m_pGraphicsQueue; }
	inline VulkanQueue * const GetComputeQueue() const { return m_pComputeQueue; }
	inline VulkanQueue * const GetTransferQueue() const { return m_pTransferQueue; }
	inline VulkanQueue * const GetPresentationQueue() const { return m_pPresentationQueue; }

	inline const VkDevice Get() const { return m_Device; }
	inline operator VkDevice() const { return m_Device; }

private:
	const VulkanGPUInfo & m_GPUInfo;

	VkDevice m_Device = VK_NULL_HANDLE;

	// Device queues
	VulkanQueue * m_pGraphicsQueue		= nullptr;
	VulkanQueue * m_pComputeQueue		= nullptr;
	VulkanQueue * m_pTransferQueue		= nullptr;
	VulkanQueue * m_pPresentationQueue	= nullptr;
};