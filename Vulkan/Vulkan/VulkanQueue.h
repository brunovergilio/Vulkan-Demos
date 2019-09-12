#pragma once


#include "VulkanDevice.h"


struct SubmitInfo : VkSubmitInfo
{
	SubmitInfo(const uint32_t waitSemaphoreCount, const VkSemaphore * const  pWaitSemaphores,
		const uint32_t signalSemaphoreCount, const VkSemaphore * const pSignalSemaphores,
		const uint32_t commandBufferCount, const VkCommandBuffer * const pCommandBuffers,
		const VkPipelineStageFlags * const pWaitDstStageMask)
		: VkSubmitInfo{}
	{
		this->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		this->pNext = nullptr;
		this->waitSemaphoreCount = waitSemaphoreCount;
		this->pWaitSemaphores = pWaitSemaphores;
		this->pWaitDstStageMask = pWaitDstStageMask;
		this->commandBufferCount = commandBufferCount;
		this->pCommandBuffers = pCommandBuffers;
		this->signalSemaphoreCount = signalSemaphoreCount;
		this->pSignalSemaphores = pSignalSemaphores;
	}

	SubmitInfo() = default;
};


class VulkanQueue
{
public:
	explicit VulkanQueue(const VulkanDevice & device, const uint32_t queueIndex);
	~VulkanQueue();

	VkResult Submit(const VkCommandBuffer commandBuffer, const VkSemaphore waitSemaphore, const VkSemaphore signalSemaphore,
		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, const VkFence fence = VK_NULL_HANDLE);
	VkResult Submit(const VkSubmitInfo & submitInfo, const VkFence fence = VK_NULL_HANDLE);
	VkResult Submit(const uint32_t submitInfoCount, const VkSubmitInfo * const pSubmitInfos, const VkFence fence = VK_NULL_HANDLE);

	VkResult Present(const VkSwapchainKHR pSwapchain, const uint32_t imageIndex, const VkSemaphore waitSemaphore = VK_NULL_HANDLE);
	VkResult Present(const VkPresentInfoKHR & presentInfo);

	VkResult WaitIdle();

	inline const VkQueue Get() const { return m_Queue; }
	inline const uint32_t GetIndex() const { return m_Index; }
	inline operator VkQueue() const { return m_Queue; }

private:
	VkQueue m_Queue = VK_NULL_HANDLE;
	uint32_t m_Index = UINT32_MAX;
};